#ifndef NETWORKEDCOMPUTER_H
#define NETWORKEDCOMPUTER_H

#include "Protocol.h"
#include "Storage.h"
#include "Network.h"

template<template<class> class ProtocolPolicy,
		 class EncodingPolicy,
		 class StoragePolicy>
class NetworkedComputer: public ProtocolPolicy<EncodingPolicy>,
						 public StoragePolicy
{
public:
	// the constructor should connect this computer
	// with the network and generate some data to be sent
	// to some recipient
	NetworkedComputer(Network& n);

	~NetworkedComputer() {}

	bool connectWith(NetworkedComputer& other) {
		return false;
	}

private:
	Network& m_network;
};

#endif

//////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////// IMPLMENTATION ////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////////////

template<template<class> class ProtocolPolicy, class EncodingPolicy, class StoragePolicy>
NetworkedComputer<ProtocolPolicy, EncodingPolicy, StoragePolicy>
::NetworkedComputer(Network& net)
 : m_network(net)
{
}