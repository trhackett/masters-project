
#include <iostream>
using namespace std;

class Base {
public:
	Base() {}
};

class Derived1 : public Base {
public:
	Derived1() {}
};

class Derived2 : public Base {
public:
	Derived2() {}
};

template <class T>
class Widget {
public:
	void func() {
		cout << "generic" << endl;
	}
};

template<> void Widget<char>::func() {
	cout << "char" << endl;
}

template<> void Widget<int>::func() {
	cout << "int" << endl;
}

// I wanted this to have a Base class pointer but be
// called whenever I have a Widget<Derived> but that
// doesn't work, it's impossible. This is #3 page 7
// of Modern C++ Design. You have to use multiple inheritance
// and runtime polymorphism to achieve this effect
template<> void Widget<Base*>::func() {
	cout << "base" << endl;
}


template<class T>
class SomeCheckingPolicy {
public:
	bool doCheck(T* pointee) { return true; }
};

template<class T, template<class> class CheckingPolicy>
class Pointer : public CheckingPolicy<T> // OK that CheckingPolicy isn't an actual
{                                        // class because it's a template class so c++
public:                                  // will know the base class of object as we
    T* operator->() {                    // create them - REALLY COOL
        if (doCheck(pointee)) {
            return pointee;
        }
    }  
private:
    T* pointee;
};


int main() {
	Widget<double> dWidg;
	dWidg.func();

	Widget<char> cWidg;
	cWidg.func();

	Widget<int> iWidg;
	iWidg.func();


	// super cool
	Pointer<int, SomeCheckingPolicy> p;
}