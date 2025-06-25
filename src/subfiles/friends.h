#include "options.h"
#include "subtitle.h"
using namespace std;

string tupper(string s);
bool tf(string s);

void makeDir(string); // make a singular directory
// void makeDir(string, string, ...); // make directories
void makeDir(filesystem::path);
// void makeDir(filesystem::path, filesystem::path inputs...); // i'll do other variants later

void config(); // configuration function

void configFanmix(); // configure the fanmix

void seeMatches(smatch);

string outStrings(string, subtitle); // function call for making those damn nested file strings
string outStrings(string, subtitle, string);
string outStrings(string, subtitle, string, string);