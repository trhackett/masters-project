#ifndef APPLICATION_H
#define APPLICATION_H

#include "Protocol.h"
#include "Storage.h"
#include "DataStore.h"

#include <set>

template<template<class> class ProtocolPolicy, class EncodingPolicy,
		 class DataType, template<class> class StoragePolicy>
class Application: public ProtocolPolicy<EncodingPolicy>,
				   public StoragePolicy<DataType>
{
public:
	// the constructor should connect this computer
	// with the network and generate some data to be sent
	// to some recipient
	Application(string addr, DataStore& ds);
	string heartbeat() const;
	int connect();
	bool record(DataType data);
	int broadcast() const;

private:
	string m_address;
	DataStore& m_DataStore;

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
   : m_address(addr), m_DataStore(ds)    // init member variables
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
	m_DataStore.write(msg);
	return msg;
}

  // function that looks at the data currently in the DataStore,
  // checking for any heartbeats it doesn't already know about
template<template<class> class ProtocolPolicy, class EncodingPolicy,
		 class DataType, template<class> class StoragePolicy>
int Application<ProtocolPolicy, EncodingPolicy, DataType, StoragePolicy>

::connect()
{
	  // read the raw data
	string memData;
	m_DataStore.read(memData);

	  // go through the data, adding any heartbeat addresses to your heartbeat set
	typename ProtocolPolicy<EncodingPolicy>::MsgType msgtype;
	string data;

	  // count the number of connects added
	int numAdded = 0;
	while (!memData.empty()) {

		  // returns the index of the character past all processed
		  // data, or the length of memData if all is processed
		int idx = this->getNextMessage(memData, msgtype, data);
		
		if (msgtype == this->HEARTBEAT) {
			if (data != m_address && m_connections.insert(data).second) {
				numAdded++;
			}
		}

		  // if idx is memData.size(), this returns empty string
		memData = memData.substr(idx);
	}

	return numAdded;
}


  // store the data using your storage and return true if it was
  // successful
template<template<class> class ProtocolPolicy, class EncodingPolicy,
		 class DataType, template<class> class StoragePolicy>
bool Application<ProtocolPolicy, EncodingPolicy, DataType, StoragePolicy>

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
		DataType data = this->get(i);
		string message = this->prepareData(data.to_string());
		m_DataStore.write(message);
	}
	return this->size();
}