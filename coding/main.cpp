
#include <unistd.h> // sleep()
#include <cassert>  // assert()

#include "DataStore.h"
#include "Application.h"

void testDataStore() {
	DataStore m(5); // writes last 5 seconds

	string s;
	m.read(s);
	assert(s.size() == 0 && s == "");

	// write a bunch of strings and make sure
	// they're written
	string entire {};
	for (char i = '0'; i < '5'; i++) {
		string s1 = "iter";
		s1 += i;
		m.write(s1);
		entire += s1;
		m.read(s1);
		assert(s1 == entire);
		sleep(1);
	}
	
	// 5 seconds have passed, so iter0 is gone
	m.read(s);
	assert(s == entire.substr(5));
	sleep(1);

	// 6 seconds, iter1
	m.read(s);
	assert(s == entire.substr(10));
	sleep(1);

	// 7 seconds, iter2
	m.read(s);
	assert(s == entire.substr(15));
	sleep(1);

	// write another
	s = "final";
	m.write(s);
	entire += s;

	// 8 seconds, iter3
	m.read(s);
	assert(s == entire.substr(20));
	sleep(1);

	// 9 seconds, iter4
	m.read(s);
	assert(s == entire.substr(25));
	sleep(1);

	// final thing lasts 3 more seconds
	sleep(3);
	m.read(s);
	assert(s == "");
}

using SimpleApp = Application<SimpleProtocol,
						HuffmanEncoding,SimpleStorage>;

int main() {
	/*
	DataStore mem;

	// make some computerrs that are connected on
	// a network (more like a bus tbh)
	SimpleApp c1(mem);
	SimpleApp c2(mem);
	SimpleApp c3(mem);

	c1.connectWith(c2);
	c2.connectWith(c3);

	c1.sendData();
	c2.checkForData();

	c3.createNewData();
	*/

	testDataStore();

	cout << "Passed all tests!" << endl;
}