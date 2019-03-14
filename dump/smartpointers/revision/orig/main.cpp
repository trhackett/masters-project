#include <string>
#include <cassert>
#include <iostream>

#include "SmartPtr.h"
#include "UniquePtr.h"

using namespace std;

/*int main() {
	string* s = new string;

	*s = "tyrion";
	assert(s->size() == 6);
	assert((*s)[3] == 'i');

	cout << "Passed all tests!" << endl;
}*/

  // the user of a SmartPtr should never explicitly call
  // new or delete - SmartPtr takes care of that for us
string* allocString()       { return new string; }
void freeString(string* s)  { delete s; }

int* allocIntArray()        { return new int[10]; }
void freeIntArray(int* i)   { delete [] i; }

int main() {
	  // when initialized, SmartPtrs allocate the memory and
	  // remember how to delete it
	SmartPtr<string, UniquePtr> sptr(&allocString, &freeString);

	  // we can use SmartPtrs like raw pointers
	*sptr = "tyrion";
	assert(sptr->size() == 6);
	assert((*sptr)[3] == 'i');

	SmartPtr<string, UniquePtr> sptr1(&allocString, &freeString);
	*sptr1 = "tyrion";
	assert(sptr1 != sptr);
	assert(*sptr1 == *sptr);

	  // you can also allocate arrays
	SmartPtr<int, UniquePtr> intarrptr(&allocIntArray, &freeIntArray);

	intarrptr[0] = 10;
	intarrptr[1] = 20;
	assert((*intarrptr) + (*(intarrptr + 1)) == 30);

	int* intptr = intarrptr + 2;
	*intptr = 30;
	assert(intarrptr[0] + intarrptr[1] + intarrptr[2] == 60);

	cout << "Passed all test!" << endl;
}