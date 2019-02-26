
#include <vector>
#include <iostream>
using namespace std;

int main() {
	vector<char> v;
	for (int i = 0; i < 10; i++) {
		v.push_back(i + 'a');
	}

	char arr[5] = { 'k', 'l', 'm', 'n', 'o'};

	v.insert(v.end(), arr, arr+5);

	for (int i = 0; i < v.size(); i++) {
		cout << v[i] << " ";
	}
	cout << endl;
}