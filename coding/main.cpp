
#include <unistd.h> // sleep()

#include "Memory.h"
#include "Application.h"

using SimpleApp = Application<SimpleProtocol,
						HuffmanEncoding,SimpleStorage>;


void testMemory() {
	Memory m;

	char d1[4] = {'a','b','c','d'};
	m.write(d1, 4);

	char d2[6] = {'t','r','e','v','o','r'};
	m.write(d2, 6);

	m.printData();
	m.printEntries();

	sleep(12);

	char* d = new char[20];
	int s;
	m.read(d, s);
	delete [] d;
}

int main() {
	/*
	Memory mem;

	// make some computerrs that are connected on
	// a network (more like a bus tbh)
	SimpleApp c1(mem);
	SimpleApp c2(mem);
	SimpleApp c3(mem);

	c1.connectWith(c2);
	c2.connectWith(c3);

	c1.sendData();
	c2.checkForData();

	c3.createNewData();
	*/

	testMemory();
}