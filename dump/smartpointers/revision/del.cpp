
#include <iostream>
#include <vector>
using namespace std;

class A {
public:
	A(int r, int c)
	 : v(r, vector<int>(c, 55)), rows(r), cols(c)
	{}

	void printAll() {
		for (int i = 0; i < rows; i++) {
			for (int j = 0; j < cols; j++) {
				cout << v[i][j] << " ";
			}
			cout << endl;
		}
	}

private:
	vector<vector<int>> v;
	int rows, cols;
};

int main() {
	A a(2, 5);
	a.printAll();
}