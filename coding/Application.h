#ifndef APPLICATION_H
#define APPLICATION_H

#include "Protocol.h"
#include "Storage.h"
#include "DataStore.h"

template<template<class> class ProtocolPolicy,
		 class EncodingPolicy,
		 class StoragePolicy>
class Application: public ProtocolPolicy<EncodingPolicy>,
						 public StoragePolicy
{
public:
	// the constructor should connect this computer
	// with the network and generate some data to be sent
	// to some recipient
	Application(DataStore& n);

	~Application() {}

	bool connectWith(Application& other) {
		return false;
	}

private:
	DataStore& m_DataStore;
};

#endif

//////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// IMPLMENTATION ////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

template<template<class> class ProtocolPolicy, class EncodingPolicy, class StoragePolicy>
Application<ProtocolPolicy, EncodingPolicy, StoragePolicy>
::Application(DataStore& mem)
 : m_DataStore(mem)
{
}