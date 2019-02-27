#ifndef DATASTORE_H
#define DATASTORE_H

#include <vector>
#include <list>
#include <string>
#include <queue>
#include <iostream>
#include "timer.h"
using namespace std;

class DataStore {
public:
	DataStore(int p)
	 : m_persistence(p)
	{ }

    // write numChars characters to the network,
    // when this function completes, the network will
    // have a copy of each char between data[0] and
    // data[numChars-1] for persistence() milliseconds.
  void write(string data) {
    	// add the written data into your data
  	list<char>::iterator it = m_data.insert(m_data.end(), data.begin(), data.end());
      // push back the clean up entry into the entries.
    m_entries.push(entry(it, data.size(), m_time.elapsed()));
  }

    // read all data currently in the network,
    // when this function returns, the string s refers
    // to contains a copy of all data in DataStore
    // BIG PROBLEM WITH m_data AS LIST: this takes
    // linear time to the number of characters...!
  void read(string& data) {
      // first remove any outdated data
    cleanData();
  	  // fill their array with data
    data.clear();
    list<char>::iterator it = m_data.begin();
    for (; it != m_data.end(); ++it) {
      data.push_back(*it);
    }
  }

    // returns an integer representing the number of
    // milliseconds the network holds onto data written
    // to it for
  int persistence() const {
  	return m_persistence;
  }

  void printDataStore() {
    printData();
    printEntries();
  }

private:

	list<char> m_data;
	int m_persistence;
  Timer m_time;

  struct entry {
    list<char>::iterator start;
    int size;
    double timeEntered;
    entry(list<char>::iterator it, int s, double t)
     : start(it), size(s), timeEntered(t) {}
  };

    // need to remember where each data starts and end and
    // when it was inserted for cleaning up
  queue<entry> m_entries;

    // function that goes through the data, removing any data
    // that is too old (timeEntered > m_persistence)
  void cleanData();

    // useful helper print functions
  void printData() const;
  void printEntries() const;
};

// this runs in time linear to the number of entries and
// number of characters removed but actually still better than using
// a vector since that runs in time linear to number of
// characters removed plus number of characters after last one
// removed. So that's good.
void DataStore::cleanData() {

  // if it was entered more than m_persistence seconds (x1000 = ms) ago
  // remove it
  while (!m_entries.empty()) {
    entry& f = m_entries.front();

      // if the entry is still valid, then all the ones after it
      // are too, so stop
    if (f.timeEntered + m_persistence * 1000 > m_time.elapsed()) {
      return;
    }

      // remove all characters starting at the entries start
      // and ending after size characters have been removed
    list<char>::iterator del = f.start;
    for (int i = 0; i < f.size; i++) {
      del = m_data.erase(del);
    }

      // remove this entry and go to the next one
    m_entries.pop();
  }
}

void DataStore::printData() const {
  cout << " -- printData --" << endl;
  list<char>::const_iterator it = m_data.begin();
  for (; it != m_data.end(); ++it) {
    cout << *it;
  }
  cout << endl;
}

void DataStore::printEntries() const {
  cout << " -- printEntries --" << endl;
  queue<entry> temp(m_entries);
  int numEntries = temp.size();
  while (numEntries-- > 0) {
    entry f = temp.front();
    cout << *(f.start) << " " << f.timeEntered << " ";
  }
  cout << endl;
  cout << "totalsize: " << m_entries.size() << " data size: " << m_data.size() << endl; 
}

#endif

