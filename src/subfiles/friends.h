#include <iostream>
#include <iomanip>
#include <filesystem>
#include <vector>
#include <regex>
#include "options.h"
using namespace std;

string tupper(string s);
string toLower(string str);
bool tf(string s);

void makeDir(string); // make a singular directory
// void makeDir(string, string, ...); // make directories
void makeDir(filesystem::path);
// void makeDir(filesystem::path, filesystem::path inputs...); // i'll do other variants later

void config(); // configuration function

void configFanmix(); // configure the fanmix

void seeMatches(smatch);

filesystem::path locator(string fname, string ext);

void quickOpen(fstream &stream, string path);
void quickOpen(fstream &stream, filesystem::path path);

void showFiles(vector<filesystem::path> &, string);
void showFiles(vector<filesystem::path> &);