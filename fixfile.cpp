
#include <iostream>
#include <fstream>
#include <string>
using namespace std;

bool isWhiteSpace(char c) {
	return c == ' ' || c == '\n' || c == '\t';
}


// write a function that gets the next word to write to
// the outfile. A word is anything that is separated
// by white space on either side. nl is true if there
// is a newline character, else false.
string getNextWord(ifstream& in, bool& nl) {
	string ret = {};

	char c = 'a';
	while (in.get(c) && !isWhiteSpace(c)) {
		ret += c;
	}

	return ret;
}

void writeAllWhiteSpace(ifstream& in, ofstream& out) {
	char c;
	while (in.get(c) && isWhiteSpace(c)) {
		out << c;
	}
	in.unget();
}

void writeParagraph(ifstream& in, ofstream& out) {
	char c;
	bool oneNewLine = false;
	string word;
	while (in.get(c)) {

		if (c == '\n') {
			// if you've found one newline, remember that
			if (!oneNewLine) {
				oneNewLine = true;
			}

			// if you've found two newlines, you're done
			else {
				return;
			}
		}

		if (!isWhiteSpace(c)) {
			word += c;
		}

		else {
			out << word << ' ';
			word.clear();
		}
	}
}

int main(int argc, char* argv[]) {

	if (argc != 2) {
		cout << "need file name as arg" << endl;
		return 1;
	}

	ifstream infile(argv[1]);

	// if the file doesn't exist, stop
	if (!infile.is_open()) {
		cout << "couldn't read file " << argv[1] << endl;
		return 1;
	}

	ofstream outfile("temp");

	// if you can't create it, stop
	if (!outfile.is_open()) {
		cout << "can't write to temp" << endl;
		return 1;
	}

	char c;
	bool oneNL = false;

	while (!infile.eof()) {
		writeParagraph(infile, outfile);
		outfile << "\n\n";
	}

	infile.close();
	outfile.close();
}