#ifndef APPLICATION_H
#define APPLICATION_H

#include "Protocol.h"
#include "Storage.h"
#include "DataStore.h"

#include <set>

template<template<class> class ProtocolPolicy, class EncodingPolicy, class StoragePolicy>
class Application: public ProtocolPolicy<EncodingPolicy>, public StoragePolicy
{
public:
	// the constructor should connect this computer
	// with the network and generate some data to be sent
	// to some recipient
	Application(string addr, DataStore& ds);
	string heartbeat() const;
	int addPresentConnections();

private:
	string m_address;
	DataStore& m_DataStore;

	set<string> m_connections;
};

#endif

//////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// IMPLMENTATION ////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

template<template<class> class ProtocolPolicy, class EncodingPolicy, class StoragePolicy>
Application<ProtocolPolicy, EncodingPolicy, StoragePolicy>
::Application(string addr, DataStore& ds)
   : m_address(addr), m_DataStore(ds)    // init member variables
{
}

template<template<class> class ProtocolPolicy, class EncodingPolicy, class StoragePolicy>
string Application<ProtocolPolicy, EncodingPolicy, StoragePolicy>::heartbeat() const
{
	string msg = this->prepareDataForWrite(m_address, this->HEARTBEAT);
	m_DataStore.write(msg);
	return msg;
}

template<template<class> class ProtocolPolicy, class EncodingPolicy, class StoragePolicy>
int Application<ProtocolPolicy, EncodingPolicy, StoragePolicy>::addPresentConnections()
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
