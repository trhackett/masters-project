
#include <unistd.h>    // sleep()
#include <cassert>     // assert()
#include <type_traits> // is_trivially_destructible

#include "DataStore.h"
#include "Application.h"

void testDataStore()
{
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

using SimpleApp = Application<SimpleProtocol,HuffmanEncoding,
							char,SimpleStorage>;

void testApplication()
{
	DataStore memory(2);      // data written to memory lasts 2 seconds

	SimpleApp app1("LAX", memory);
	SimpleApp app2("CVG", memory);
	SimpleApp app3("ABQ", memory);

	  // first, we want everyone connected to memory to
	  // know about each other - so have each tell the others
	  // they're there by putting its address on memory
	const string HTBT_HDR = "HTBT";
	assert(app1.heartbeat() == HTBT_HDR + "LAX"); // make sure the message is right
	assert(app2.heartbeat() == HTBT_HDR + "CVG");
	assert(app3.heartbeat() == HTBT_HDR + "ABQ");

	  // make sure everything was written properly to the memory
	string htbts = HTBT_HDR + "LAX" + HTBT_HDR + "CVG" + HTBT_HDR + "ABQ";
	string readData;
	memory.read(readData);
	assert(htbts == readData);

	  // when they check to see who else is connected,
	  // they each discover the other two
	assert(app1.addPresentConnections() == 2);
	assert(app2.addPresentConnections() == 2);
	assert(app3.addPresentConnections() == 2);


}

int main() {

	// testDataStore(); // this passes

	testApplication();

	cout << "Passed all tests!" << endl;
}