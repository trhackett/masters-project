#include "UniquePtr.h"
#include "SharedPtr.h"
#include <iostream>

using namespace std;

int testUniquePtr() {
	int err = 0;
	return 0;
}

int testSharedPtr() {
	return 0;
}



int main() {
	int err = 0;

	err = testUniquePtr();
	if (err > 0) {
		cout << "UNIQUE failed test #" << err << endl;
		return 0;
	}

	err = testSharedPtr();
	if (err > 0) {
		cout << "SHARED failed test #" << err << endl;
	}

	cout << "Passed all tests!" << endl;
}