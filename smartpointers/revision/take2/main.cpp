
#include "LeaklessPtr.h"

#include <iostream>
using namespace std;

int main() {
	LeaklessPtr<int, BasePtrImpl> iPtr(
		[] () { return new int; },
		[] (int* i) { delete i; });
}