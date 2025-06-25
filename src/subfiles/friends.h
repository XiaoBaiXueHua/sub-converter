#include "options.h"
using namespace std;

string tupper(string s);
bool tf(string s);

string rgba(string); // takes a string as the ass file takes it and converts it to rgba
int htod(string c);
void makeDir(string); // make a singular directory
// void makeDir(string, string, ...); // make directories
void makeDir(filesystem::path);
// void makeDir(filesystem::path, filesystem::path inputs...); // i'll do other variants later

void config(); // configuration function

void configFanmix(); // configure the fanmix


string outStrings(string, subtitle); // function call for making those damn nested file strings
string outStrings(string, subtitle, string);
string outStrings(string, subtitle, string, string);