#include "UniquePtr.h"
#include <iostream>
#include <string>
using namespace std;

/*
	UniquePtr doesn't do += for an array
*/

int main() {
	
	UniquePtr<string> smPtr(new string("trevor"),
						   [] (string* s) { delete s; });
	cout << '"' << *smPtr << "\" has " << smPtr->size() << " characters" << endl;
	*smPtr = "hackett";
	cout << "Now it is " << *smPtr << endl;

	UniquePtr<int> smPtr1(new int[10],
					    [] (int* i) { delete [] i; });
	for (int i = 0; i < 10; i++) {
		smPtr1[i] = i*i;
	}

	cout << "smPtr1 has the elements:";
	for (int i = 0; i < 10; i++) {
		cout << " " << smPtr1[i];
	}
	cout << endl;

	cout << "element in position 3 is " << *(smPtr1 + 3) << endl;

	cout << "\n\n";

	UniquePtr<char> charPtr(new char('Z'),
		           [] (char* c) { delete c; });

	// smPtr1++; // this will ALWAYS cause an error
	             // because when you move the pointer,
	             // and it gets freed, it will delete the
	             // wrong thing automatically
	cout << "smPtr1 at " << &smPtr1 << " with val " << *smPtr1 \
		<< ", next addr is " << &smPtr1 + 1 << " with val " << *(smPtr1 + 1) << endl;

	// smPtr1 += 1; // this will ALWAYS cause problems for the
		            // same reason as above
	cout << "smPtr1 now at " << *smPtr1 << endl;
	
	UniquePtr<char> charPtrMoved(move(charPtr));
	cout << "Moved '" << *charPtrMoved << '\'' << endl;

	// UniquePtr<char> BAD(charPtrMoved); //compiler error
}