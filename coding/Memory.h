#ifndef MEMORY_H
#define MEMORY_H

#include <vector>
#include <list>
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

const int PERSISTENCE = 10;

class Memory {
public:
	Memory()
	 : m_persistence(PERSISTENCE)
	{ }

    // write numChars characters to the network,
    // when this function completes, the network will
    // have a copy of each char between data[0] and
    // data[numChars-1] for persistence() milliseconds.
  void write(char* data, int numChars) {
    	// add the written data into your data
  	list<char>::iterator it = m_data.insert(m_data.end(), data, data + numChars);
      // push back the clean up entry into the entries.
    m_entries.push_back(entry(it, numChars, m_time.elapsed()));
    cout << "pushed back " << *it << endl;
  }

    // read all data currently in the network,
    // when this function returns, the array pointed
    // to by data will have numChars characters
    // from data[0] to data[numChars-1]. You can't
    // change the data the you read, read-only.
    // BIG PROBLEM WITH m_data AS LIST: this takes
    // linear time to the number of characters...!
  void read(const char* data, int& numChars) {
      // first remove any outdated data
    cleanData();
    	// know how much is sent back
  	numChars = m_data.size();
  	// fill their array with data
  }

    // returns an integer representing the number of
    // milliseconds the network holds onto data written
    // to it for
  int persistence() const {
  	return m_persistence;
  }

  void printData() {
    cout << " -- printData --" << endl;
    list<char>::iterator it = m_data.begin();
    for (; it != m_data.end(); ++it) {
      cout << *it;
    }
    cout << endl;
  }

  void printEntries() {
    cout << " -- printEntries --" << endl;
    int totalsize = 0;
    list<entry>::iterator it = m_entries.begin();
    for (; it != m_entries.end(); ++it) {
      cout << *(it->start) << " " << it->timeEntered << " ";
      totalsize += it->size;
    }
    cout << endl;
    cout << "totalsize: " << totalsize << " " << "data size: " << m_data.size() << endl;
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
};

// this runs in time linear to the number of entries and
// number of characters removed but actually still better than using
// a vector since that runs in time linear to number of
// characters removed plus number of characters after last one
// removed. So that's good.
void Memory::cleanData() {
  list<entry>::iterator it = m_entries.begin();
  while (it != m_entries.end()) {
      // if it was entered more than 10 seconds (10,000 ms) ago
      // remove it
    if (it->timeEntered + 10000 < m_time.elapsed()) {
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

#endif

