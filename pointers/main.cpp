
#include "LeaklessPtr.h"

#include <type_traits>
#include <iostream>
#include <cassert>
using namespace std;

struct Number {
	int n;
	Number() : n(0) {}
	void increment() { n++; }
};

using IntSelfishPtr = LeaklessPtr<int, SelfishPtrImpl>;
using NumberSelfishPtr = LeaklessPtr<Number, SelfishPtrImpl>;

IntSelfishPtr func1() {
	return IntSelfishPtr(
		[] () { return new int; },
		[] (int* i) { delete i; });
}

void testSelfish() {

	// constructor takes in two lambda functions for allocating
	// memory and freeing it.
	IntSelfishPtr iptr1(
		[] () { return new int; },
		[] (int* i) { delete i; });

	// copy constructor (elided) should steal resources from the
	// IntSelfish Ptr in func1()
	IntSelfishPtr iptr2 = func1();

	// moving is also stealing.
	IntSelfishPtr iptr3(std::move(iptr1));

	assert(iptr1 == nullptr);
	assert(iptr2 != nullptr);
	assert(iptr3 != nullptr);

	// like any normal pointer, you can set the value and access
	// it later using the * operator or the [] operator.
	*iptr2 = 10;
	assert(*iptr2 == 10);
	assert(iptr2[0] == 10);

	*iptr3 = 10;
	assert(*iptr2 == *iptr3);

	// also works with user-defined types
	NumberSelfishPtr nptr(
		[] () { return new Number; },
		[] (Number* n) { delete n; });

	// the -> operator can be used to access members
	// of that type
	assert(nptr != nullptr);
	nptr->increment();
	nptr->increment();

	assert(nptr->n == 2);

	iptr1 = iptr2; // steal

	assert(iptr2 == nullptr);
	assert(iptr1 != nullptr);
	assert(*iptr1 == 10);

	IntSelfishPtr iptr4(iptr3);

	assert(iptr4 != nullptr);
	assert(iptr3 == nullptr);
	assert(*iptr4 == 10);
}

using IntAltruisticPtr = LeaklessPtr<int, AltruisticPtrImpl>;
using NumberAltruisticPtr = LeaklessPtr<Number, AltruisticPtrImpl>;

void testAltruistic() {

	// constructed similarly - we tell the Ptr how to allocate and
	// free memory
	IntAltruisticPtr iptr1(
		[] () { return new int; },
		[] (int* i) { delete i;});

	*iptr1 = 10;
	assert(iptr1 != nullptr);
	assert(*iptr1 == 10);

	IntAltruisticPtr iptr2 = iptr1;
	assert(iptr2 != nullptr);
	assert(*iptr2 == 10);
	assert(iptr2 == iptr1);

	IntAltruisticPtr iptr3 = std::move(iptr1);
	assert(iptr1 == nullptr);
	assert(iptr1 != iptr2);
	assert(iptr3 != nullptr);
	assert(iptr3[0] == 10);

	NumberAltruisticPtr nptr1(
		[] () { return new Number; },
		[] (Number* n) { delete n; });

	NumberAltruisticPtr nptr2(
		[] () { return new Number; },
		[] (Number* n) { delete n; });

	nptr1 = nptr2;

	nptr1->increment();
	nptr2->increment();
	nptr1->increment();

	assert(nptr1 == nptr2);
	assert(nptr1->n == 3);
}

int main() {

	testSelfish();
	testAltruistic();

	cout << "Passed all tests!" << endl;
}