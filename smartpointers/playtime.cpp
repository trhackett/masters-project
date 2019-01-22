#include <iostream>
#include <memory>

using namespace std;

class Base1 {
public:
	Base1() {};
	void nonvirtfunc() { cout << "TTT" << endl; }
	virtual void virtfunc() = 0;
private:
	int i;
};
class Derived1 : public Base1 {
public:
	Derived1() {}
	void virtfunc() {}
private:
	int j;
};

class Base {
public:
	Base(int i) { j = ++i; }
private:
	int j;
};

class Derived : public Base {
public:
	Derived() : Base(1) { num = 0; }
	Derived(Derived& other) : Base(1) {
		num = other.num;
	}
private:
	int num;
};

int main()
{
	Derived d;
	Derived d1(d);


	int a = 10;
	int* aptr = &a;

	  // compiles fine but spits out some
	  // garbage value
	// cout << aptr[3] << endl;

	aptr = new int(12);
	unique_ptr<int> aunique(aptr);
	int* aptr2 = aptr;
	// cout << *aunique << endl; // this prints 12 but also causes
	                             // a double freeing... Why?
	delete aptr2;
	cout << *aunique << endl;

	  // so this compiles fine but throws a runtime error
	  // of double free - so unique_ptrs really aren't very
	  // good...? they can't even know if two things are
	  // pointing to the same place
	// unique_ptr<int> uni2(aptr);

	[] () { cout << "lambda1" << endl; };   // this line does nothing
	                                        // provides a nameless function
	                                        // and doesn't call it
	[] () { cout << "lambda2" << endl; }(); // but this line calls that function

	[] (int a) { cout << "lambda" << a << endl; }(3); // this one gives an argument
	                                                  // and calls the function with it

	struct weirdStruct {
		// int& ten;  // storing a reference makes it
		              // so when you initialize a weirdStruc
		              // you must provide something for ten
		              // to refer to
		int ten; // this is ok obviously
	};

	weirdStruct wS;

	class Dummy {
	public:
		Dummy() {};
	private:
		int m;
		// Dummy d; // this is bad
		Dummy* d;   // this is OK
	}; // definition isn't complete until here

	Dummy dum;


	Derived1 d2;
	d2.virtfunc();
	d2.nonvirtfunc();

}

/*
[]	Capture nothing
[&]	Capture any referenced variable by reference
[=]	Capture any referenced variable by making a copy
[=, &foo]	Capture any referenced variable by making a copy,
            but capture variable foo by reference
[bar]	Capture bar by making a copy; don't copy anything else
[this]	Capture the this pointer of the enclosing class
*/