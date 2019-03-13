#include <vector>
#include <iostream>
using namespace std;

struct intStruct {
	int val;
	intStruct&& operator+(intStruct iS) {
		return move(intStruct(val + iS.val));
	}
	intStruct(int i) : val(i) { }
};

void func(int i) {

}

int& func2(int& i) {
	return i;
}

int&& func3(int i) {
	return move(i);
}

class Manatee {
public:
	Manatee() { }
};

class T {
public:
	T() { cout << "default" << endl; }
	T(T&&) = delete; // { cout << "move" << endl; }
	T(T&) = delete; // {cout << "copy" << endl; }
};

/*
	The following code works because of the following (decltype)
4) If the argument is any other expression of type T, and
 a) if the value category of expression is xvalue, then decltype yields T&&;
 b) if the value category of expression is lvalue, then decltype yields T&;
 c) if the value category of expression is prvalue, then decltype yields T.
*/
template <class T> struct value_category   { static constexpr char const* value = "prvalue"; };
template <class T> struct value_category<T&>  { static constexpr char const* value = "lvalue"; };
template <class T> struct value_category<T&&> { static constexpr char const* value = "xvalue"; };
#define PRINT_VALUE_CAT(expr) std::cout << #expr << " is " << value_category<decltype((expr))>::value << '\n'
/*  these functions came from this:
https://medium.com/@barryrevzin/value-categories-in-c-17-f56ae54bccbe
*/

// proposal to add rvalue reference:
//  http://www.open-std.org/jtc1/sc22/wg21/docs/papers/2004/n1690.html

int f(int&& d) { PRINT_VALUE_CAT(d); return 10; }

int main() {
	vector<int> intVec(10);
	// intVec.size() = 10;
	vector<int> intVvec2 = vector<int>();

	int x = 10;


	int y = move(x);
	// the same as int y = static_cast<int&&>(x);
	// int&& is an rvalue reference of type int
	// move() produces an xvalue, take in a prvalue

	int& xx = x;   // lvalue reference
	int&& xxx = move(x); // rvalue reference
	int&& xy = 10;
	// cout << x << " " << xxx << " " << xy << endl; // 10 10 10

	PRINT_VALUE_CAT(xx);
	PRINT_VALUE_CAT(xxx);
	PRINT_VALUE_CAT(y);
	PRINT_VALUE_CAT(move(x));
	PRINT_VALUE_CAT(cout);
	PRINT_VALUE_CAT(intVec.size());
	PRINT_VALUE_CAT(intVec.at(0));
	PRINT_VALUE_CAT(vector<int>());

	enum Color { red, blue, yellow };
	PRINT_VALUE_CAT(red);

	f(move(xxx));

	cout << x << " " << y << endl;

	x++;

	cout << x << " " << y << endl;

	//C++11 move semantics in action
	string source("abc"), target;
	target = move(source);  //pilfer source
	//source no longer owns the resource
	cout<<"source: "<<source<<endl; //source:
	cout<<"target: "<<target<<endl; //target: abc

	//T var = T();  // doesn't compile when move constructor is deleted
	              // because T() is a prvalue and this is move-
	              // contruction. So what I want this to do is create a
				  // temporary T (prvalue) and move it into var. The compiler
				  // is required to omit the move constructor here but even
				  // so there is no move constructor to omit (I deleted it)
				  // so it still says "use of deleted function" talking about
				  // the move constructor
				  // APPARENTLY this is supposed to be ok in c++17 but when I
				  // set std=c++17, it still doesn't compile so...?

	// cout << (&T) << endl; // class names are not EXPRESSIONS
	PRINT_VALUE_CAT(func(10));
	PRINT_VALUE_CAT(func);

	int g = 10;
	PRINT_VALUE_CAT(func2(g));
	PRINT_VALUE_CAT(func2(g) + 2);

	PRINT_VALUE_CAT(Manatee());
	PRINT_VALUE_CAT(x + x);

	int&& xrref = 0;
	PRINT_VALUE_CAT(xrref);

	PRINT_VALUE_CAT(func3(x));
	int ret = func3(x);

	intStruct iS1(4);
	intStruct iS2(5);
	intStruct&& iS3 = iS1 + iS2;
	cout << iS3.val << endl;
	PRINT_VALUE_CAT(iS1 + iS2);
	PRINT_VALUE_CAT(iS3);
	PRINT_VALUE_CAT(g++);

	int something = func3(10);
}