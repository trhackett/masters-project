#ifndef AIRPORT_H
#define AIRPORT_H

#include "Application.h"

struct Route {
	int numStops;
	string codes;
	Route(string c) : codes(c), numStops(c.size()/3) {}

	string to_writeable() {
		return codes;
	}
};

class Airport :
	public Application<SimpleProtocol,HuffmanEncoding,Route,SimpleStorage>
{
public:
	Airport(string addr, DataStore& ds, const vector<Route>& routes);
	void alertAirports() const;
	void gatherData();
};

#endif

Airport::Airport(string addr, DataStore& ds, const vector<Route>& routes)
 : Application<SimpleProtocol,HuffmanEncoding,Route,SimpleStorage>(addr, ds)
{
	  // store all your routes
	for (int i = 0; i < routes.size(); i++) {
		record(routes[i]);
	}
}

void Airport::alertAirports() const
{
	heartbeat();
	broadcast();
}

void Airport::gatherData()
{
	  // if there's anyone you don't know about,
	  // add their routes to yours
	if (connect() > 0) {
		readMessages();
	}
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

void testRealApplication()
{
	DataStore datastore(2);

	map<string,vector<Route>> airport_route_map;
	readInRoutes(airport_route_map, "routes.txt");

	vector<Airport> airportVec;

	  // make an airport for each one read in from the input file
	map<string,vector<Route>>::iterator it = airport_route_map.begin();
	for (; it != airport_route_map.end(); ++it) {

		  // push a new one back
		airportVec.push_back(Airport(it->first, datastore, it->second));

		  // get the one you just inserted
		Airport& airport = airportVec[airportVec.size()-1];
	}

	  // map so "ABQ" < "CVG" < "LAX"
	Airport& lax = airportVec[2];
	Airport& cvg = airportVec[1];
	Airport& abq = airportVec[0];

	  // join network of airports communicating and post
	  // the routes starting at you so that others can factor
	  // that into their decisions
	lax.alertAirports();
	cvg.alertAirports();
	abq.alertAirports();

	  // connect to the others and gather their data
	lax.gatherData();
	cvg.gatherData();
	abq.gatherData();
}

