#ifndef ENCODE_H
#define ENCODE_H

#include <string>
using namespace std;

class SimpleEncoding {
public:
	string encode(string data) const;
};

class HuffmanEncoding {
public:
	string encode(string data) const;
};

#endif

string SimpleEncoding::encode(string data) const
{
	return data;
}

string HuffmanEncoding::encode(string data) const
{
	return data;
}