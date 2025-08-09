// friend functions that are also dependent on the subtitle class to avoid recursive inclusion headers
#include <iostream>
#include <iomanip>
#include <filesystem>
#include <string>
#include <vector>
#include <regex>
#include "options.h"
#include "subtitle.h"

using namespace std;

string outStrings(string, subtitle); // function call for making those damn nested file strings
string outStrings(string, subtitle, string);
string outStrings(string, subtitle, string, string);