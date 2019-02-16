#ifndef NETWORK_H
#define NETWORK_H


/*
	The only thing the Network knows how to do is store data for some amount
	of time. When anyone tries to access some data, it gives it to them. It 
	me be best to implement as a map of addresses (destination) to data. That
	way, whenever someone writes, it adds something to the map and whenver
	someone else reads, it gives them back the data they were looking for.
*/

class Network {
public:
	Network() {}
	~Network() {}

private:
};

#endif
