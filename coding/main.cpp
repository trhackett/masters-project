
#include "Network.h"

using SimpleComputer = NetworkedComputer<SimpleProtocol,
						HuffmanEncoding,SimpleStorage>;

int main() {
	Network net;

	// make some computerrs that are connected on
	// a network (more like a bus tbh)
	SimpleComputer c1(&net);
	SimpleComputer c2(&net);
	SimpleComputer c3(&net);

	c1.connectWith(c2);
	c2.connectWith(c3);

	c1.sendData();
	c2.checkForData();

	c3.createNewData();
}