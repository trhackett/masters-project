#include "SmartPtr.h"
#include "UniquePtr.h"
#include "SharedPtr.h"

#include <iostream>
#include <string>
#include <vector>
using namespace std;

int* intAlloc() {
	return new int;
}

void intDelete(int* i) {
	delete i;
}

int testUniquePtr() {

	
	{ // integer pointer created and deleted

		  // 1: can allocate an integer
		SmartPtr<int, UniquePtr> sptr (
			[] () { return new int; },   // allocate a new integer
			[] (int* i) { delete i; } ); // delete an allocated integer

		if (sptr == nullptr) {
			return 1;
		}

		  // 2: can assign value
		*sptr = 10;
		if (*sptr != 10) {
			return 2;
		}

		SmartPtr<int, UniquePtr> iptr (
			[] () { return new int; },
			[] (int* i) { delete i; } );

		  // initialized to nullptr
		if (iptr == nullptr) {
			return 3;
		}

		  // not pointing to the same thing
		if (sptr == iptr || iptr == sptr) {
			return 4;
		}

		  // swap
		*iptr = 11;

		sptr.swap(iptr);

		if (*iptr != 10 || *sptr != 11) {
			return 5;
		}

		iptr.swap(sptr); // once done, iptr->11 sptr->10

		if (*iptr != 11 || *sptr != 10) {
			return 6;
		}

		  // assignment operator
		iptr = sptr;

		  // sptr no longer points to anything (unique)
		if (sptr != nullptr) {
			return 7;
		}

		  // iptr has value sptr used to have
		if (*iptr != 10) {
			return 8;
		}

		  // assign to itself does nothing
		SmartPtr<int, UniquePtr>* iptrptr = &iptr;

		iptr = iptr;

		if (*iptr != 10) {
			return 9;
		}

		if (iptrptr != & iptr) {
			return 10;
		}

		  // reset
		sptr.reset(new int(20), [] (int* i) { delete i; });
		if (*sptr != 20) {
			return 11;
		}

		iptr.reset(new int(30), [] (int* i) { delete i; });
		if (*iptr != 30) {
			return 12;
		}

		  // seems a huge problem that you can get access
		  // to the memory here !!!
		int* intptr = iptr.operator->();
		if (*intptr != 30) {
			return 13;
		}

		  // move constructor
		SmartPtr<int, UniquePtr> movedPtr(move(iptr));

		if (iptr != nullptr) {
			return 14;
		}

		if (*movedPtr != 30) {
			return 15;
		}

	} // done with integer pointers

	{ // string array

		  // create and delete a string
		SmartPtr<string, UniquePtr> strptr(
			[] () { return new string[10]; },
			[] (string* s) { delete [] s; }  );

		if (strptr == nullptr) {
			return 16;
		}

		strptr[0] = "cercei";
		strptr[1] = "tyrion";
		strptr[2] = "tywin";

		if (strptr[0] != "cercei" ||
			strptr[1] != "tyrion" ||
			strptr[2] != "tywin")
		{
			return 17;
		}

		strptr[6] = "jaime";
		string* sptr = strptr + 6;
		if (*sptr != "jaime") {
			return 18;
		}

		*sptr = "hodor";
		if (strptr[6] != "hodor") {
			return 19;
		}

		SmartPtr<string, UniquePtr> singleptr([] (string* s) { delete s; });
		string* s = new string(strptr[1]);
		singleptr.reset(s, [] (string* s) { delete s; });

		if (*singleptr != "tyrion") {
			return 20;
		}

		if (singleptr->size() != 6) {
			return 21;
		}

		(*singleptr) += " lannister";

		if (*singleptr != "tyrion lannister") {
			return 22;
		}

		  // didn't change other pointer (allocated new object)
		if (strptr[1] != "tyrion") {
			return 23;
		}

		strptr.reset(new string[20], [] (string* s) { delete [] s; });
		if (strptr == nullptr) {
			return 24;
		}

		if (strptr[0] == "cercei") {
			return 25;
		}

		strptr[1] = "neddard";

		  // thief constructor
		SmartPtr<string, UniquePtr> strptrcopy(strptr);

		if (strptr != nullptr) {
			return 26;
		}

		if (strptrcopy == nullptr) {
			return 27;
		}

		if (strptrcopy[0] != "" ||
			strptrcopy[1] != "neddard")
		{
			return 28;
		}

	} // done with string array
	
	return 0;
}

int testSharedPtr() {

	{  // shared pointer to a char

		SmartPtr<char, SharedPtr> cptr(
			[] () { return new char; },
			[] (char* c) { delete c; });

		if (cptr == nullptr) {
			return 1;
		}

		*cptr = 'A';

		if (*cptr != 'A') {
			return 2;
		}

		if (cptr.getNumReferences() != 1) {
			return 3;
		}

		/*
			Ok there's a trade off here. I can either keep smart pointers
			noncopyable and force cptr2 to be default constructed and then
			operator= to share them. This prevents me from having to make
			changes to unique pointer - which we don't want to be able to 
			make copies of - using the copy constructor would effectively
			steal ownership.

			The other option is to make smart pointers copyable. This allows
			us to make this call below but forces us to provide a copy
			constructor for unique pointer, which would give ownership to
			the newly created smart pointer. I'm going to go with this option
			becuase it gives us some more flexibility.
		*/
		SmartPtr<char, SharedPtr> cptr2(cptr);

		if (*cptr2 != 'A') {
			return 4;
		}

		if (cptr2.getNumReferences() != 2 ||
			cptr.getNumReferences() != 2)
		{
			return 5;
		}

		if (cptr != cptr2) {
			return 6;
		}

		*cptr2 = 'Z';

		if (*cptr != 'Z' ||
			*cptr2 != 'Z')
		{
			return 7;
		}


		SmartPtr<char, SharedPtr> cptr3(
			[] () { return new char; },
			[] (char* c) { delete c; });

		if (cptr3 != nullptr) {
			return 8;
		}

		cptr3 = cptr2;

		if (*cptr3 != 'Z') {
			return 9;
		}

		if (cptr3.getNumReferences() != 3) {
			return 10;
		}

		cptr.reset(new char, [] (char* c) { delete c; });
		*cptr = 'M';

		if (cptr == nullptr) {
			return 11;
		}

		if (cptr.getNumReferences() != 1) {
			return 12;
		}

		if (cptr3.getNumReferences() != 2) {
			return 13;
		}

		if (*cptr != 'M') {
			return 14;
		}

	}  // end shared pointer to a char


	{  // shared pointer to object array
		struct Coord {
			int x, y;
		};

		SmartPtr<Coord, SharedPtr> coordArray(
			[] () { return new Coord[5]; },
			[] (Coord* c) { delete [] c; });

		coordArray[0].x = 5;
		coordArray[0].y = 10;

		Coord* c = &coordArray[0];
		if (c->x != 5 || c->y != 10) {
			return 15;
		}

		SmartPtr<Coord, SharedPtr> movee(std::move(coordArray));

		if (coordArray != nullptr) {
			return 16;
		}

		if (movee.getNumReferences() != 1) {
			return 17;
		}

		  // has been stolen
		if (coordArray.getNumReferences() != -1) {
			return 18;
		}

		Coord* c2 = &movee[3];
		c2->x = 8;
		c2->y = 14;

		if ((movee + 3)->x != 8 ||
			(movee + 3)->y != 14)
		{
			return 19;
		}

	}  // free object array

	{

		SmartPtr<int, SharedPtr> intptr(
			&intAlloc, &intDelete);

		*intptr = -5;

		if (*intptr != -5) {
			return 20;
		}

		for (int i = 0; i < 100; i++) {
			SmartPtr<int, SharedPtr> cpy(intptr);

			if (*cpy != -5) {
				return 21;
			}

			if (cpy.getNumReferences() != 2) {
				return 22;
			}
		}
	}

	return 0;
}

int main() {
	int err = 0;

	err = testUniquePtr();
	if (err > 0) {
		cout << "UNIQUE PTR ERROR: " << err << endl;
		return 0;
	}

	err = testSharedPtr();
	if (err > 0) {
		cout << "SHARED PTR ERROR: " << err << endl;
		return 0;
	}

	cout << "PASSED ALL TESTS!!!" << endl;
	return 0;
}