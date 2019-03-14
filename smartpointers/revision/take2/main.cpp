
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

	// can't copy construct, can't default construct
	static_assert(!is_copy_constructible<IntSelfishPtr>::value, "Can't copy construct SelfishPtr");
	static_assert(!is_default_constructible<IntSelfishPtr>::value, "Can't default construct SelfishPtr");

	IntSelfishPtr iptr1(
		[] () { return new int; },
		[] (int* i) { delete i; });

	IntSelfishPtr iptr2 = func1();

	IntSelfishPtr iptr3(std::move(iptr1));

	assert(iptr1 == nullptr);
	assert(iptr2 != nullptr);
	assert(iptr3 != nullptr);

	*iptr2 = 10;
	assert(*iptr2 == 10);
	assert(iptr2[0] == 10);

	*iptr3 = 10;
	assert(*iptr2 == *iptr3);

	NumberSelfishPtr nptr(
		[] () { return new Number; },
		[] (Number* n) { delete n; });

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