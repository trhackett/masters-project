
#include <iostream>
#include <fstream>
#include <string>
using namespace std;


// write a function that gets the next word to write to
// the outfile. A word is anything that is separated
// by white space on either side.
string getNextWord(ifstream& in) {
	string ret = {};

	char c = 'a';
	while (!in.eof() && c != ' ') {
		in.get(c);
		ret += c;
	}

	return ret;
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

	// I want to copy in all characters from infile to
	// outfile until I run out in infile but I want to
	// only write 60 chars/line
	bool lastWasSpace = false;
	while (!infile.eof()) {

		// write a line
		int charsWrote = 0;
		string writeToNextLine = {};
		while (charsWrote < 60 && !infile.eof()) {

			// only first iteration, clear it
			if (!writeToNextLine.empty()) {
				outfile << writeToNextLine;
				writeToNextLine.erase(writeToNextLine.begin(),
									  writeToNextLine.end());
			}

			// get the next word
			string s = getNextWord(infile);

			if (s == "\n") {
				outfile << '\n';
				break;
			}

			// if it doesn't fit, save it for the next line
			if (charsWrote + s.size() >= 60) {
				writeToNextLine = s;
				break;
			}

			// don't write a bunch of newlines for spaces
			if (s == " ") {
				lastWasSpace = true;
			} else {
				lastWasSpace = false;
			}

			if (lastWasSpace && s == " ") {
				continue;
			}

			outfile << s;
			charsWrote += s.size();
		}
	}

	infile.close();
	outfile.close();
}