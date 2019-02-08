#include <iostream>
using namespace std;


struct S {
	// lots of dynamically allocated data
	S() { cout << "default" << endl; }
	S(const S& other) { cout << "copy" << endl; }
	S(S&& other) { cout << "move" << endl; }
	int m;
};

S& func1() {
	static S s;
	// do some computation, build up s,
	// s is really big
	return s;
}

S&& func2() {
	S s;
	// do some computation, build up s,
	// s is really big
	return std::move(s);
}


int main() {
	S s_copy = func1();
	S s_move = func2();
}