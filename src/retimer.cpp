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

using namespace std;

string tmp{""}, tmp2{""};

int main() {
	config(); // configure it
	cout << "Configuration complete." << endl;
	filesystem::path subfile{"../dummyfiles/[Patjantess] Fushigi Yugi Ep01 Aegis Edit.ass"};
	subtitle s(subfile);
	// s = loca
	cout << s.name() << "." << s.ext() << endl;
	return 0;
}