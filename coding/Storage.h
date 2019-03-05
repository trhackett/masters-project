#ifndef STORAGE_H
#define STORAGE_H

#include <vector>
using namespace std;

template<class DataType>
class SimpleStorage {
public:
	int      store(DataType data);
	int      size() const       { return m_vec.size(); }
	DataType get(int idx) const { return m_vec[idx].value; }

private:
	struct Data {
		DataType value;
		bool valid;
		Data(DataType v) : value(v), valid(true) {}
	};

	vector<Data> m_vec;
};

#endif

template<class DataType>
int SimpleStorage<DataType>::store(DataType data)
{
	m_vec.push_back(Data(data));
	return m_vec.size()-1;
}