#ifndef APPLICATION_H
#define APPLICATION_H

#include "Protocol.h"
#include "Storage.h"
#include "DataStore.h"

#include <set>

  // four policies: Protocol, Encoding, DataType, Storage
  // Protocol and Storage are class templates - Protocol needs
  // an Encoding class and Storage needs a DataType specified.
template<template<class> class ProtocolPolicy, class EncodingPolicy,
		 class DataType, template<class> class StoragePolicy>
class Application: public ProtocolPolicy<EncodingPolicy>,
				   public StoragePolicy<DataType>
{
public:
	  // every Application knows it's own address and
	  // the DataStore that it's connected to.
	Application(string addr, DataStore& ds);

	  // heartbeat is how the Application makes its presence
	  // known to other Applications on the DataStore - it posts
	  // it's own address on the DataStore for others to discover.
	  // The return value is the string message that is written
	  // for testing purposes.
	string heartbeat() const;

	  // connect searches the DataStore for heartbeat messages,
	  // and adds the address in the message to it's connections
	  // if they are not already connected. Since an Application
	  // that doesn't have a heartbeat message on the DataStore
	  // is no longer considered available, my connections should
	  // be cleared every time I check for new connections. This
	  // function returns the number of connects added. 
	int connect();

	  // record is how the Application learns new data - all new
	  // data is stored and the index of the stored object is
	  // returned. This should be made more general, but it's not
	  // clear to me how atm.
	int record(DataType data);

	  // broadcast writes all stored data to DataStore in a separate
	  // data message (determined by the Protocol). It returns the
	  // number of messages written to DataStore.
	int broadcast() const;

	  // readMessages reads all data messages on the DataStore, that
	  // were not put there by me. There are added to our Storage
	  // (stored locally) and the number of messages stored is returned.
	int readMessages();

private:
	string m_address;
	DataStore& m_datastore;

	  // log time to know if I already have a connection and easy
	  // to erase.
	set<string> m_connections;
};

#endif

//////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// IMPLMENTATION ////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////


  // intiitalize any private member variables
template<template<class> class ProtocolPolicy, class EncodingPolicy,
		 class DataType, template<class> class StoragePolicy>
Application<ProtocolPolicy, EncodingPolicy, DataType, StoragePolicy>

::Application(string addr, DataStore& ds)
   : m_address(addr), m_datastore(ds)    // init member variables
{

}

  // function that write this Application's address on the DataStore
  // so that other Applications can connect with it
template<template<class> class ProtocolPolicy, class EncodingPolicy,
		 class DataType, template<class> class StoragePolicy>
string Application<ProtocolPolicy, EncodingPolicy, DataType, StoragePolicy>

::heartbeat() const
{
	string msg = this->prepareHeartbeat(m_address);
	m_datastore.write(msg);
	return msg;
}

  // function that looks at the data currently in the DataStore,
  // checking for any heartbeats it doesn't already know about
template<template<class> class ProtocolPolicy, class EncodingPolicy,
		 class DataType, template<class> class StoragePolicy>
int Application<ProtocolPolicy, EncodingPolicy, DataType, StoragePolicy>

::connect()
{
	  // clear old connections
	m_connections.clear();
	  // read the raw data
	string memData;
	m_datastore.read(memData);

	  // go through the data, adding any connections to your connections set
	  // count the number of connects added, start reading rawdata from first
	  // element - getNextConnection returns true if there is another connection
	int numAdded = 0, idx = 0;
	string data;
	while (this->getNextConnection(idx, memData, data)) {
		
		  // try to add it to your connections, set::insert.second is true
		  // on successful insertion
		if (data != m_address && m_connections.insert(data).second) {
			numAdded++;
		}
	}

	return numAdded;
}


  // store the data using your storage and return true if it was
  // successful
template<template<class> class ProtocolPolicy, class EncodingPolicy,
		 class DataType, template<class> class StoragePolicy>
int Application<ProtocolPolicy, EncodingPolicy, DataType, StoragePolicy>

::record(DataType data)
{
	return this->store(data);
}

  // broadcast all of the stored data and return how much was sent
template<template<class> class ProtocolPolicy, class EncodingPolicy,
		 class DataType, template<class> class StoragePolicy>
int Application<ProtocolPolicy, EncodingPolicy, DataType, StoragePolicy>

::broadcast() const
{
	for (int i = 0; i < this->size(); i++) {
		  // data from Storage
		DataType data = this->get(i);
		  // prepare message with header
		string message = this->prepareData(data.to_writeable(), m_address);
		  // write to DataStore
		m_datastore.write(message);
	}

	  // return number of Data elements written
	return this->size();
}


  // read all of the data from the DataStore and store it locally
template<template<class> class ProtocolPolicy, class EncodingPolicy,
		 class DataType, template<class> class StoragePolicy>
int Application<ProtocolPolicy, EncodingPolicy, DataType, StoragePolicy>

::readMessages()
{
	string rawdata;
	m_datastore.read(rawdata);

	string data, addr;
	int idx = 0, numMsgs = 0;

	  // read the messages one by one, stopping when you've processed
	  // all the raw data using the protocol
	while (this->getNextData(idx, rawdata, data, addr)) {
		  // store it if it isn't your data
		if (addr != m_address) {
			numMsgs++;
			this->store(data);
		}
	}

	return numMsgs;
}