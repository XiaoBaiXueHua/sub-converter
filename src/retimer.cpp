#include <iostream>
#include <iomanip>
#include "math.h"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <regex>
#include <filesystem>
#include "./subfiles/subtitle.h"
#include "subfiles/subtitle.cpp"
#include "subfiles/friends.h"
#include "subfiles/friends.cpp"
#include "subfiles/sub-friends.h"
#include "subfiles/sub-friends.cpp"

using namespace std;

string tmp{""}, tmp2{""};
vector<cue> lint = {}; // cue vector
fstream istr, ostr;
int main()
{
	config(); // configure it
	cout << "Configuration complete." << endl;
	filesystem::path subfile{"../dummyfiles/[Patjantess] Fushigi Yugi Ep01 Aegis Edit.ass"};
	subtitle s(subfile); // make a subtitle class out of it
	cout << s.name() << "." << s.ext() << endl;
	quickOpen(istr, s.path());
	quickOpen(ostr, string(outStrings(options::output, s) + ".ass"));

	// int l{0}; // tracks how many lines it's been
	while (getline(istr, tmp))
	{
		if (tmp != "[Events]")
		{
			ostr << tmp << endl;
		} else {
			break;
		}
	}
	getline(istr, tmp);
	ostr << tmp << endl; // print that line too bc we're just copying ehe
	while (getline(istr, tmp)) {
		// now we um. now we just push everything into a vector <3
		// cue c(tmp); 
		lint.push_back(cue(tmp)); 
	}

	// if the cues are less than 0.4s aparts, combine them
	return 0;
}