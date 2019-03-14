
#include "LeaklessPtr.h"

#include <iostream>
using namespace std;

LeaklessPtr<int, SelfishPtrImpl> func1() {
	return LeaklessPtr<int, SelfishPtrImpl>(
		[] () { return new int; },
		[] (int* i) { delete i; });
}

int main() {
	LeaklessPtr<int, SelfishPtrImpl> iPtr(
		[] () { return new int; },
		[] (int* i) { delete i; });

	LeaklessPtr<int, SelfishPtrImpl> iPtr2 = func1();
}