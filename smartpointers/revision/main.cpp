
#include <iostream>
#include <cassert>
#include "Ptr.h"

using namespace std;

int test() {
	LeakLessPtr<int> pInt(
		[] () { return new int; },
		[] (int* i) { delete i; });

	if (pInt == nullptr) {
		return 1;
	}

	*pInt = 10;

	if (*pInt != 10) {
		return 2;
	}

	LeakLessPtr<int> pInt1(
		[] () { return new int; },
		[] (int* i) { delete i; });

	if (pInt == pInt1) {
		return 3;
	}

	if (!(pInt != pInt1)) {
		return 4;
	}

	return -1;
}

int main() {

	int t1 = test();

	if (t1 != -1) {
		cout << "Failed on " << t1 << endl;
	} else {
		cout << "Passed all tests!" << endl;
	}
}
