
# Value Categories

The c++ standard specifies that "An expression is a sequence of operators and operands that specifies a computation". An expression can result in a value and can cause side effects. Expressions can be as simple as `i++`; and as complicated as `[] (double d, S s) -> Prime { d += 7+5+3+2; return s.compute(d,d*2,d*3,d*5,d*7); }`. Every c++ expression is defined by two independent things: type and value category. Since you have a lot of experience working with type, this lab focuses on value category, a concept with which you've likely interacted very little.

The most straight-forward way to tackle value categories is to first look at the assignment operator, and a program like the following very simple one will be instructive:

```
int i;
i = 10; // i.operator=(10);
```

When the assignment operator is invoked, there is a clear left side (i) and right side (10). Each side is a distinct expression, and therefore has its own type (which, if the two types are not the same, some conversion needs to happen) and its own value category. Given this, we'll break expressions down into glvalues and rvalues. You may be wondering why the first branch here wouldn't be into r(ight)values and l(eft)values. Well for one we're going to defer the name lvalue for a later branch, you'll see. For two, the name lvalue actually refers to C++'s predecessor: C, which defined expressions to be lvalues and everything else. In this paradigm, the right hand side would define some computation and the left hand side would define some location to store the result in.

When c++ came around, it turned functions into lvalues (gave them an addressable location) and brought references (&) into the picture. Functions as lvalues was an important shift because it allowed us to use and pass around pointers to them. References are an important concept in c++ (hopefully you already have some idea why) but they bind to lvalues, rather than rvalues. Expanding on the code above paints a picture:

```
int i;
i = 10;        // operator=(int),  i is lvalue,    10 is rvalue
int& iRef = i; // operator=(int&), iRef is lvalue, i is lvalue
```
  
Up until c++11, this was fine, it was everything we needed: lvalues and rvalues were sufficient to define the value category of every possible expression. But when move semantics and rvalue references came into the picture, there was a whole category of expressions that no longer fit cleanly into lvalue or rvalue, but kind of was both. Let's look at a motivating example.

```
struct S {
  // lots of 
S() { ... }                   // line A
S(const S& other) { ... }     // line B
S(S&& other) { ... }          // line C
~S() { ... }

};

S& func1() {                    // line D
static S s; // (note 1)       -- invokes line A
  // do some computation,
  // build up s,
  // s is really big
return s;                     // invokes line B
}

S&& func2() {                   // line E
S s;                          // invokes line A
  // do some computation,
  // build up s,
  // s is really big
return std::move(s);          // invokes line C
}


int main() {
S s_copy = func1();           // invokes line D
S s_move = func2();           // invokes line E
}
```

When the call to func1 is made in the main function and the result stored into s_copy, the following happens: the body of func1 executes returning an object of type S, the copy constructor for S is called (note 1 for details) - the const S& argument passed in being the object created by func1, the body of the copy constructor executes making a copy of what func1 had created. Effectively, the object that we want is being created two times: once by func1 and then again by the copy constructor. If the goal of func1() is to create an object but have only the variable s_copy refer to that object, then this code is very wasteful. Assuming the building up of an S object is expensive, this operation has to wastefully happen twice.

C++11 fixed this with move semantics, which avoids making copies of objects with dynamically allocated memory in the case that a variable is about to be destroyed but before it does, we only want another variable (lvalue) to refer to to that object. When the call to func2 is made, the following happens: the body of func2 executes returning an rvalue reference effectively calling a different S constructor - the move constructor, s within func2() is destroyed. The move constructor, rather than making deep copies of all of that dynamically allocated memory, simply has s_move's internal pointers point to that memory (and has s's pointers point to nullptr to prevent it from freeing the data when it is destroyed).

But what does all of this have to do with value categories? Well move semantics introduced a new type: an rvalue reference (&&). These aren't exactly just rvalues because they have an addressible location in memory but they aren't exactly just lvalues because they disappear after use. As soon as s from func2 has been moved into s_move, s refers no longer exists. Because of this, c++11 introduced a third value category, xvalue. As before, each category is mutually exclusive - no expression can belong to more than one at a time. We've branched our previous {lvalue,rvalue} categories into the following: {lvalue,xvalue,prvalue}.

```
        expression
        /        \
     glvalue   rvalue
      /   \    /   \
 lvalue   xvalue   prvalue
```

The strict definitions of these are as follows (from the c++ standard):
- A glvalue is an expression whose evaluation determines the identity of an object, bit-field, or function.
- An rvalue is a prvalue or an xvalue.
- A prvalue is an expression whose evaluation initializes an object or a bit-field, or computes the value of the operand of an operator, as specified by the context in which it appears.
- An xvalue is a glvalue that denotes an object or bit-field whose resources can be reused (usually because it is near the end of its lifetime). [ Example: Certain kinds of expressions involving rvalue references yield xvalues, such as a call to a function whose return type is an rvalue reference or a cast to an rvalue reference type. -end example ]
- An lvalue is a glvalue that is not an xvalue.

Analyze the following code and answer the questions below:

```
{
1      int i = 0;                      
2      int j = i+2;                    

3      vector intVec(10, i);      
4      for (; i < intVec.size();       
5           i++)                       
       {
           ...
       }

6      int& iref = i;                  
7      int&& jref = 0;                 
8      vector vec = vector() 

9      int func(double d) {            
10       return d*2;                    
       }

11     func(10)                         

12     enum Color { red, yellow, blue };

       class Manatee { ... };
       void func3(Manatee m) { ... }
13     func3(Manatee());                        

       int arr[5];
14     *(arr+3) = 3;                            
15     arr[4] = 3;                              

       int& func2(int i) {
         return i;
       }

       int g = 10;
16     func2(g);                                
17     func2(g) + 2;                            

       int&& func4(int i) {
18       return move(i);                        
       }

19     func4(g);                                
20     int&& rref = static_cast(g)       

       struct intStruct {
         int val;
         intStruct&& operator+(intStruct iS) {
           return intStruct(val + iS.val);
         }
         intStruct(int i) : val(i) { }
       };

       struct intStruct {
         int val;
 
         intStruct&& operator+(intStruct iS) {
21         return move(intStruct(val + iS.val));
         }
        
         intStruct(int i) : val(i) { }
       };

       intStruct iS1(4);
       intStruct iS2(5);
22     intStruct&& iS3 = iS1 + iS2;

       struct S {
         int m;
       };

       S&& func5();

23     func5().m;                  
24     int n = S().m;              

       int arr[10];
25     int* p = arr;               

26     int (*pFunc) (double) = func

27     void func6(int&& i) { ... } 
28     func6(n);                   
29     func6(10);                  

       void func7(double d) {
         cout << d << endl;
       }

       double func3() {
         double x(5.8);
         return x;
       }

30     func7(func3());              
}
```
For the following questions, indicate if the expression from the code above is a (A) lvalue, (B) xvalue, (C) prvalue.

```
 line    expression
--------------------
  1         i
  1         0
  2         i+1
  2         j
  3         intVec
  4         intVec.size()
  5         i++
  6         iref
  6         i
  7         jref
  8         vector()
  9         func
  9         d
  10        d*2
  11        func(10)
  12        red
  13        Manatee()
  14        (arr+3)
  14        *(arr+3)
  15        arr[4]
  16        func2(g)
  17        func3(g) + 2
  18        move(i)
  19        func4(g)
  20        static_cast(g)
  21        intStruct(val + iS.val)
  22        iS1 + iS2
  22        iS3
  23        func5()
  23        func5().m
  24        S()
  24        S().m
  25        arr
  26        func
  27        i    // with function call on line 28
  27        i    // with function call on line 29
  30        func3()
```

*note 1*: this example is somewhat contrived and probably not the best motivating example for move semantics but it gets to the point. When the object of type S in func1 is declared static, it persists outside of the function call. This prohibits c++ from performing copy elision in, which it only calls the default constructor one time, the result being referred automatically by the variable s_copy as opposed to calling the default constructor, making an object for s, returning it into the copy constructor that makes a copy of s for s_copy.

