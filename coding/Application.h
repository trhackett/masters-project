#ifndef APPLICATION_H
#define APPLICATION_H

#include "Protocol.h"
#include "Storage.h"
#include "Memory.h"

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
	Application(Memory& n);

	~Application() {}

	bool connectWith(Application& other) {
		return false;
	}

private:
	Memory& m_memory;
};

#endif

//////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// IMPLMENTATION ////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

template<template<class> class ProtocolPolicy, class EncodingPolicy, class StoragePolicy>
Application<ProtocolPolicy, EncodingPolicy, StoragePolicy>
::Application(Memory& mem)
 : m_memory(mem)
{
}