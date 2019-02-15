#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "Encode.h"

/*
	SimpleProtocol is just that, simple.
	You can just send straight away and it is guaranteed
	that the receiver is going to receive.
*/
template<class EncodingPolicy>
class SimpleProtocol : public EncodingPolicy {
public:
	SimpleProtocol() {}
	~SimpleProtocol() {}

	void sendData() {
		this->encodeData(); // IMPORTANT, I read somewhere why this is necessary
		                    // and can't remember
	}
	void checkForData() {}
};

#endif