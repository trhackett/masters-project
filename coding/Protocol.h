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

	  // write
	string prepareHeartbeat(string addr) const;
	string prepareData(string data, string addr) const;
	  // read
	bool getNextConnection(int& startIdx, const string& rawData, string& addr) const;
	bool getNextData(int& startIdx, const string& rawData, string& data, string& addr) const;

private:
	// we need a series of types of headers that the Application can
	// access to tell us what type of message is being sent, which we
	// will use to format the message itself
	enum MsgType { HEARTBEAT, DATA, KEY };

	const map<MsgType,string> m_headers;

	  // take in some raw data, a start idx, and a MsgType
	  // searches the rawdata for a header matching that msgtype
	  // returns true if it finds it, setting idx to the first character
	  // of that header, else false
	bool getNextHeaderIdx(int& idx, const string& rawData, MsgType msgtype) const;
	int getDataSize(int& start, const string& rawdata) const;
};

#endif

template<class EncodingPolicy>
SimpleProtocol<EncodingPolicy>::SimpleProtocol()
 : m_headers({{HEARTBEAT,"HTBT"},{DATA,"DATA"},{KEY,"DKEY"}})
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
	msg += this->encode(addr, nullptr);

	return msg;
}

template<class EncodingPolicy>
bool SimpleProtocol<EncodingPolicy>::

getNextHeaderIdx(int& idx, const string& rawData, MsgType msgtype) const
{
	  // if we're looking out of bounds, return false
	if (idx >= rawData.size()) {
		return false;
	}

	  // get the potential header starting at each char, see if it's
	  // a match and if you find one, return true
	for (; idx != rawData.size(); idx++) {
		string nextFour = rawData.substr(idx, 4);

		if (nextFour == m_headers.at(msgtype)) {
			return true;
		}
	}

	  // if it's nowhere in the rawdata, return false
	return false;
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

  // getNextConnection(0, "HTBTLAXHTBTCVG", data)
  //    sets startIdx to 7 and data to LAX and
  //    returns true
  // getNextConnection(0, "DATAcvgaopmsn", data)
  //    returns false
template<class EncodingPolicy>
bool SimpleProtocol<EncodingPolicy>::

getNextConnection(int& startIdx, const string& rawData, string& addr) const
{
	  // if there's another hearbeat message to get, get it
	  // and return true
	if (getNextHeaderIdx(startIdx, rawData, HEARTBEAT)) {
		addr = rawData.substr(startIdx+4, 3); // addr is the next three past the header
		addr = this->decode(addr, nullptr);   // decode
		startIdx += 7;                        // processed 7 chars
		return true;
	}

	  // no messages found, so we've processed all the data and not found
	  // a message of type msgtype
	return false;
}

template<class EncodingPolicy>
bool SimpleProtocol<EncodingPolicy>::

getNextData(int& startIdx, const string& rawData, string& data, string& addr) const
{
	  // check if the data header is in there anywhere
	if (getNextHeaderIdx(startIdx, rawData, DATA)) {
		startIdx += 4;                               // move past the header
		addr = rawData.substr(startIdx, 3);          // get the address
		startIdx += 3;                               // move past the address
		int size = getDataSize(startIdx, rawData);   // get data's size
		startIdx += size;                            // move past the size
		data = rawData.substr(startIdx, size);       // get the data
		data = this->decode(data, nullptr);          // decode
		return true;
	}

	  // if data header isn't in there, no data
	return false;
}


template<class EncodingPolicy>
string SimpleProtocol<EncodingPolicy>::prepareData(string data, string addr) const
{
	string msg = m_headers.at(DATA) + addr;
	msg += to_string(data.size());
	msg += ',';
	msg += this->encode(data, nullptr);
	return msg;
}


template<class EncodingPolicy>
int SimpleProtocol<EncodingPolicy>::

getDataSize(int& start, const string& rawdata) const {
	string ssize;
	  // comma delimited, make sure it's always a digit
	for (; rawdata[start] != ',' && rawdata[start] >= '0' && rawdata[start] <= '9'; start++) {
		ssize += rawdata[start];
	}

	return stoi(ssize);
}