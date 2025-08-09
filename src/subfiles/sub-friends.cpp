// friend functions that are also dependent on the subtitle class
#include <iostream>
#include <iomanip>
#include <filesystem>
#include <vector>
#include <regex>
#include "options.h"
#include "subtitle.h"

using namespace std;

string outStrings(string folder, subtitle sub, string appendum, string prependum) // appendum then prependum bc there's more likely to be smth appended
{
	string t{""};
	if (options::output.string() != "")
	{
		// if we have a proper output folder
		t += options::output.string() + "/";
	}

	if (folder != "")
	{
		t += folder + "/";
	}

	if (prependum != "")
	{
		t += prependum + "-";
	}

	t += sub.name();

	if (appendum != "")
	{
		t += "-" + appendum;
	}
	// return string(t + ".vtt");
	return t;
}

string outStrings(string folder, subtitle sub, string appendum)
{
	return outStrings(folder, sub, appendum, "");
}
string outStrings(string folder, subtitle sub)
{
	return outStrings(folder, sub, "", "");
}