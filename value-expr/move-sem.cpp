#include <iostream>
using namespace std;

  // class that allocated memory
class BasicDynArr {
public:
	BasicDynArr(int n) 
	 : m_cap(n), m_arr(new int[m_cap]), m_size(0)
	{
	}

	~BasicDynArr() {
		if (m_arr != nullptr) {
			delete [] m_arr;
		}
	}

	BasicDynArr(const BasicDynArr& other)
	 : m_cap(other.m_cap), m_arr(new int[m_cap]), m_size(other.m_size)
	{
		cout << "copy" << endl;
		for (size_t i = 0; i < m_size; i++) {
			m_arr[i] = other.m_arr[i];
		}
	}

	BasicDynArr(BasicDynArr&& other)
	 : m_cap(other.m_cap), m_arr(other.m_arr), m_size(other.m_size)
	{
		other.m_arr = nullptr;
	}

	BasicDynArr& operator=(const BasicDynArr& rhs) {
		if (this != &rhs) {
			m_cap = rhs.m_cap;
			m_size = rhs.m_size;
			m_arr = new int[m_cap];

			for (size_t i = 0; i < m_size; i++) {
				m_arr[i] = rhs.m_arr[i];
			}
		}
	}

	void insert(int val) { m_arr[m_size++] = val; }
	int getValAt(int idx) const { return m_arr[idx]; }

private:
	int  m_cap;
	int* m_arr;
	int  m_size;
};


BasicDynArr createArray(int size) {
	BasicDynArr bda(size);
	return move(bda);
}


int main() {
	// BasicDynArr bad = createArray(10);

	BasicDynArr one(10);
	one.insert(4);
	cout << one.getValAt(0) << endl;
	BasicDynArr two(move(one));
	// cout << one.getValAt(0) << endl;  // now segfaults, which is good
	cout << two.getValAt(0) << endl;
}