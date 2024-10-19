#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <regex>
#include <filesystem>
#include "./subfiles/subtitle.h"

using namespace std;

filesystem::path subfile{""}, directory{".."};
fstream istr, ostr; // io file streams
vector<filesystem::path> available;

// function loop for showing the available ass and srt files
void showFiles(); 
void SRtoVT();
void quickOpen(fstream &stream, string path);
void quickOpen(fstream &stream, filesystem::path path);

int main() {
	// filesystem::directory_entry currDir{directory}; // turn the string into a directory lol
	cout << "Files available for conversion:" << endl;
	showFiles();
	int choice{0};
	cout << "Please choose a file to convert: ";
	cin >> choice;
	while (choice > available.size()) {
		cout << "hey you can't do that. choose another number: ";
		cin >> choice;
	}
	if (cin.good()) {
		subfile = available[choice - 1]; // set the subfile to the chosen one
	}

	subtitle subs(subfile); // makes a subtitle object with the chosen file
	cout << "Now working with " << subs.name() << "." << subs.ext() << endl;
	// cout << "("<< subs.path() <<")\n";
	quickOpen(istr, subs.path()); // open the relevant subtitle file
	quickOpen(ostr, string("../output/" + subs.name() + ".vtt")); // make the output file
	ostr << "WEBVTT\n\n\n"; // write this at the top
	if (subs.ext() == "srt") {
		SRtoVT();
	}

	return 0;
}

void quickOpen(fstream &stream, string path) {
	stream.open(path);
	if (!stream.is_open()) {
		stream.clear();
		stream.open(path, ios::out); // makes the file if it's not already open
		stream.close();
		stream.open(path);
	} else {
		cout << "this stream is already open." << endl;
	}
	
	cout << "opened " << path << " via string.\n";
	// quickOpen(stream, path);
}
// overloading for path object types
void quickOpen(fstream &stream, filesystem::path path) {
	stream.open(path);
	if (!stream.is_open()) {
		stream.clear();
		stream.open(path, ios::out); // makes the file if it's not already open
		stream.close();
		stream.open(path);
	}
	cout << "opened " << path << " via path.\n";
}

void showFiles() {
	int i{1};
	available.clear(); // clears the vector of available files
	for (auto const &dir_entry : filesystem::recursive_directory_iterator{filesystem::directory_entry{directory}}) {
		if (!dir_entry.is_directory()) {
			filesystem::path currPath{dir_entry.path()}; // turn it into a path object
			// cout << "File type " << currPath.extension() << endl;
			if (currPath.extension() == ".ass" || currPath.extension() == ".srt") {
				cout << setw(15) << i << ".\t" << currPath << endl;
				available.push_back(currPath);
				i++;
			}
		}
	}
}

void SRtoVT() {
	string tmp{""}; // tmp for the getline
	bool timingLine{false}; // boolean for whether the following line has the timing in it
	int currLine{1}, tmpInt{0}; // integer for the current line in the .srt file, as well as a temp for checking if we're currently 

	while (getline(istr, tmp)) {
		try {
			tmpInt = stoi(tmp);
			// cout << tmp;
		} catch(exception) {
			if (int(char(tmp[0])) < 0) {
				// don't know how this happens, but it seems to be the issue with ep 60's subs so.
				tmpInt = currLine; // for now, we'll just assume that we're supposed to be starting a new line
			} else {
				// this is commented out for now, but it's used to debug what's wrong with the line. 
				// if (regex_search(tmp, regex("\\d+"))) {
				// 	cout << "int(" << int(char(tmp[0])) << "): " << char(tmp[0]) << endl << tmp << endl;
				// }
			}
		}
		if (tmpInt == currLine) {
			cout << "Line " << currLine << ".\n";
			timingLine = true; // if this current number is the same as what line we're supposed to be on, set this to true
			currLine++; // and iterate this for the next loop
		} else if (timingLine) {
			ostr << regex_replace(tmp, regex(","), ".") << endl; // now, if it's a timing line, we replace the commas with periods and spit the line out into the new vtt file
			timingLine = false; // and then set this to false so that on the next loop we get 
		} else {
			ostr << tmp << endl; // and then write the line to the file
		}
	}
	cout << "Complete." << endl;
	// close the streams lol
	istr.close();
	ostr.close();
}