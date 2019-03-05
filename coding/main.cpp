
#include <unistd.h>    // sleep()
#include <cassert>     // assert()
#include <type_traits> // is_trivially_destructible
#include <fstream>     // ifstream

#include "DataStore.h"
#include "Airport.h"

void testSimpleApplication();
void testDataStore();

int main()
{
	testDataStore();

	testSimpleApplication();

	// once both of those functions pass, you know that your Application
	// class can successfully read from and write to the DataStore. Of
	// course, we haven't done any kind of interesting encoding.
	// Let's get to that.

	testRealApplication();

	cout << "Passed all tests!" << endl;
}


/*
	Function that tests a very simple version of the application. It defines
	a SimpleApp that uses the most reduced versions of each Policy, creates
	three instances of SimpleApp with addresses, has them connect to the DataStore
	with a heartbeat, learn about the existence of the others, write some data
	to the DataStore, and receive each other's data.

	If you changed minor things based on intentional ambiguities in the spec,
	that's fine. You may have to adjust these tests as well.
*/
void testSimpleApplication()
{
	  // Character is a simple wrapper around char. We need
	  // to provide a to_writeable function that returns a
	  // string for any type that is going to be used by the
	  // Application.
	struct Character {
		char c;
		Character(string s) : c(s[0]) {}
		Character(char ch) : c(ch) {}

		string to_writeable() {
			return string {c};
		}
	};

	using SimpleApp = Application<SimpleProtocol,SimpleEncoding,
							  Character,SimpleStorage>;

	DataStore memory(2); // data written to memory lasts 2 seconds

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
	assert(app1.connect() == 2);
	assert(app2.connect() == 2);
	assert(app3.connect() == 2);

	  // make some arbitrary data
	string app1Data = "kylie";
	for (int i = 0; i != app1Data.size(); i++) {
		assert(app1.record(app1Data[i]) >= 0);
	}

	string app2Data = "kim";
	for (int i = 0; i != app2Data.size(); i++) {
		assert(app2.record(app2Data[i]) >= 0);
	}

	string app3Data = "khloe";
	for (int i = 0; i != app3Data.size(); i++) {
		assert(app3.record(app3Data[i]) >= 0);
	}

	  // everyone writes all of their data to the DataStore
	assert(app1.broadcast() == app1Data.size());
	assert(app2.broadcast() == app2Data.size());
	assert(app3.broadcast() == app3Data.size());

	  // and everyone reads all data that isn't their own.
	assert(app1.readMessages() == app2Data.size() + app3Data.size());
	assert(app2.readMessages() == app1Data.size() + app3Data.size());
	assert(app3.readMessages() == app1Data.size() + app2Data.size());
}


/*
	This function tests the functionality of the DataStore class.
	In particular, it ensures that data actually only persists
	for the number of seconds passed in to the Constructor.
*/
void testDataStore()
{
	cout << "please hold for 15 seconds" << endl;

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