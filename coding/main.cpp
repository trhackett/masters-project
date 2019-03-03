
#include <unistd.h>    // sleep()
#include <cassert>     // assert()
#include <type_traits> // is_trivially_destructible
#include <fstream>     // ifstream

#include "DataStore.h"
#include "Application.h"

struct Route {
	int numStops;
	string codes;
	Route(string c) : codes(c), numStops(c.size()/3) {}
	string to_string() const { return codes; }
};

void testApplication();
void testDataStore();
void readInRoutes(map<string,vector<Route>>& arMap, string file);

void testRealApplication()
{
	using AirportApp = Application<SimpleProtocol,HuffmanEncoding,
									   Route,SimpleStorage>;

	DataStore datastore(2);

	map<string,vector<Route>> airport_route_map;
	readInRoutes(airport_route_map, "routes.txt");

	vector<AirportApp> airportVec;

	  // make an airport for each one read in from the input file
	map<string,vector<Route>>::iterator it = airport_route_map.begin();
	for (; it != airport_route_map.end(); ++it) {

		  // push a new one back
		airportVec.push_back(AirportApp(it->first, datastore));

		  // get the one you just inserted
		AirportApp& airport = airportVec[airportVec.size()-1];

		  // add the codes to it
		for (int i = 0; i < it->second.size(); i++) {
			assert(airport.record(it->second[i]));
		}
	}

	  // map so "ABQ" < "CVG" < "LAX"
	AirportApp& lax = airportVec[2];
	AirportApp& cvg = airportVec[1];
	AirportApp& abq = airportVec[0];

	  // heartbeat so datastore knows about them for a while
	const string HTBT_HDR = "HTBT";
	assert(lax.heartbeat() == HTBT_HDR + "LAX");
	assert(cvg.heartbeat() == HTBT_HDR + "CVG");
	assert(abq.heartbeat() == HTBT_HDR + "ABQ");

	  // connect to the others
	assert(lax.connect() == 2);
	assert(cvg.connect() == 2);
	assert(abq.connect() == 2);

	  // broadcast all data
	assert(lax.broadcast() == airport_route_map["LAX"].size());
	assert(cvg.broadcast() == airport_route_map["CVG"].size());
	assert(abq.broadcast() == airport_route_map["ABQ"].size());
}

int main()
{
	// testDataStore();   // pass

	// testApplication(); // pass

	// once both of those functions pass, you know that your Application
	// class can successfully read from and write to the DataStore. Of
	// course, we haven't done any kind of interesting encoding, nor are
	// we storing any data at the application. Let's get to that.

	testRealApplication();

	cout << "Passed all tests!" << endl;
}



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

void testApplication()
{
	using SimpleApp = Application<SimpleProtocol,SimpleEncoding,
							  char,SimpleStorage>;

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
	assert(app1.connect() == 2);
	assert(app2.connect() == 2);
	assert(app3.connect() == 2);
}

void readInRoutes(map<string,vector<Route>>& arMap, string file)
{
	ifstream input(file);

	if (input.is_open()) {

		while (!input.eof()) {
			getline(input, file);

			if (!file.empty()) {
				// get the index of the first space, take all
				// characters up to that space - that's the airport code
				int spaceIdx = file.find(' ');
				string code = file.substr(0, spaceIdx);
				int numRoutes = stoi(file.substr(spaceIdx+1));

				arMap[code] = {}; // init empty vector

				while (numRoutes-- > 0) {
					getline(input, file);
					arMap[code].push_back(Route(file));
				}
			}
		}
	}
}