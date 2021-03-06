#ifndef ENCODE_H
#define ENCODE_H

#include <string>
using namespace std;

class SimpleEncoding {
public:
	string encode(string data, char* tree) const { return data; }
	string decode(string data, char* tree) const { return data; }
};

/*
	Deferred to make other specs and learn new c++ concepts rather
	than writing code for something I'm already familiar with. It's
	a good exercise to actually code up this algorithm.
*/
class HuffmanEncoding {
public:
	string encode(string data, char* tree) const { return data; }
	string decode(string data, char* tree) const { return data; }
	char* getKey() const {}

private:
	  // to settle our concerns about dynamically allocating
	  // character arrays to pass the trees around, you can
	  // push a tree converted to character array into this
	  // vector and set tree encode function argument to point
	  // to the correct element in this vector. This is optional.
	vector<vector<char>> m_trees;
};

#endif