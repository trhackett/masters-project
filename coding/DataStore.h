#ifndef DATASTORE_H
#define DATASTORE_H

#include <vector>
#include <list>
#include <string>
#include <iostream>
#include "timer.h"
using namespace std;

/*
	The only thing the Network knows how to do is store data for some amount
	of time. When anyone tries to access some data, it gives it to them. It 
	me be best to implement as a map of addresses (destination) to data. That
	way, whenever someone writes, it adds something to the map and whenver
	someone else reads, it gives them back the data they were looking for.
*/


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
    m_entries.push_back(entry(it, data.size(), m_time.elapsed()));
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
  list<entry> m_entries;

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
  list<entry>::iterator it = m_entries.begin();
  while (it != m_entries.end()) {
      // if it was entered more than m_persistence seconds (x1000 = ms) ago
      // remove it
    if (it->timeEntered + m_persistence * 1000 < m_time.elapsed()) {
        // remove all characters starting at the entries start
        // and ending after size characters have been removed
      list<char>::iterator del = it->start;
      for (int i = 0; i < it->size; i++) {
        del = m_data.erase(del);
      }

        // remove this entry and go to the next one
      it = m_entries.erase(it);
    }

      // if removed nothing, check the next one
    else {
      ++it;
    }
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
  int totalsize = 0;
  list<entry>::const_iterator it = m_entries.begin();
  for (; it != m_entries.end(); ++it) {
    cout << *(it->start) << " " << it->timeEntered << " ";
    totalsize += it->size;
  }
  cout << endl;
  cout << "totalsize: " << totalsize << " " << "data size: " << m_data.size() << endl;
}

#endif

