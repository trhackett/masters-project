#ifndef PROTOCOL_H
#define PROTOCOL_H

#include "Encode.h"
#include <map>

/*
	SimpleProtocol is just that, simple.
	You can just send straight away and it is guaranteed
	that the receiver is going to receive.
*/
template<class EncodingPolicy>
class SimpleProtocol : public EncodingPolicy {
public:
	// know what the header strings are for the types
	SimpleProtocol();

	// we need a series of types of headers that the Application can
	// access to tell us what type of message is being sent, which we
	// will use to format the message itself
	enum MsgType { HEARTBEAT, DATA };

	string prepareHeartbeat(string addr) const;
	string prepareData(string data) const;
	int getNextMessage(const string& rawData, MsgType& msgtype, string& data) const;

private:
	const map<MsgType,string> m_headers;
};

#endif

template<class EncodingPolicy>
SimpleProtocol<EncodingPolicy>::SimpleProtocol()
 : m_headers({{HEARTBEAT,"HTBT"},{DATA,"DATA"}})
{

}


template<class EncodingPolicy>
string SimpleProtocol<EncodingPolicy>::
prepareHeartbeat(string addr) const
{
	  // the first four characters of the message specify the
	  // type of message that it is - that's always true
	string msg = m_headers.at(HEARTBEAT);

	  // IMPORTANT, I read somewhere why this is necessary
	  // to call the template base class's function but can't remember
	msg += this->encode(addr);

	return msg;
}

  // this function takes in the raw data and gives the Application
  // the next message in the rawData - all messages start with a 4
  // character header - HTBT or DATA. Once it finds a message, it
  // sets msgtype and data to the corresponding values and returns
  // the index representing one character passed the rawData that has
  // been processed. So if characters 0 through 6 are processed, it
  // returns 7. If no message is found, it returns the size of the
  // rawData string that is passed in - that is, it tells the Application
  // that it processed all the data

  // getNextMessage("HTBTLAXHTBTCVG", msgtype, data)
  //    sets msgtype to HEARTBEAT and data to LAX and
  //    returns 7
template<class EncodingPolicy>
int SimpleProtocol<EncodingPolicy>::
getNextMessage(const string& rawData, MsgType& msgtype, string& data) const
{
	  // we treat it as an error if an empty rawData has been passed in, return -1
	if (rawData.empty()) {
		return -1;
	}

	  // if rawData is ok, process it
	for (int i = 0; i != rawData.size(); i++) {

		  // get the next four characters
		string nextFour = rawData.substr(i, 4);

		  // if HEARTBEAT
		if (nextFour == "HTBT") {
			msgtype = HEARTBEAT;            // heartbeat msg
			data = rawData.substr(i+4, 3);  // addr is the next three
			return i+7;                     // 7 characters past i processed
		}
	}

	// no messages found, so we've processed all the data
	return rawData.size();
}


template<class EncodingPolicy>
string SimpleProtocol<EncodingPolicy>::prepareData(string data) const
{
	string msg = m_headers.at(DATA);
	msg += this->encode(data);
	return msg;
}