
#include "Ptr.h"

int main() {
	CleanlyPtr<int> pInt(
		[] () { return new int; },
		[] (int* i) { delete i; });
}
