// simple read-only class for the subtitle file types

#include <iostream>
#include <iomanip>
#include <string>
#include <set>
#include <vector>
#include <fstream>
#include <sstream>
#include <regex>
#include <filesystem>
#include "options.h"

#ifndef AEGISUBTITLES
#define AEGISUBTITLES

using namespace std;

string toLower(string str);

class subtitle
{
public:
	subtitle() {}; // default constructor
	subtitle(filesystem::path file);
	subtitle(vector<string> &csv); // this version of the constructor requires you to locate the path in the main

	string name();
	string ext();
	filesystem::path path();
	string title();
	string track();
	string artist();
	string url();
	void setPath(filesystem::path pith);
	void setPath(string pith);
	static string pathify(string str);
	static string pathify(filesystem::path file);

private:
	string e{"ass"}, n{""}, t{""}, a{""}, u{""}, trackNum{"01"}; // the filename, extension, and then the song title, artist, n url; default extension "ass"
	filesystem::path p{""};										 // the relative path
};

class subTime // this is basically just an elaborate float from a string. tbh.
{
public:
	subTime() {}; // default constructor
	subTime(string t);

	subTime(float f); // constructor from the float duration
	// overload the operators
	friend ostream &operator<<(ostream &, const subTime &);
	// friend istream& operator>>(istream&, const subTime&);
	float operator+(const subTime &operand2) const; // you are only being included so that operator- will not cry
	float operator-(const subTime &operand2) const; // we only need subtraction for this it'll be fine
	friend inline bool operator==(const subTime &, const subTime &);
	friend inline bool operator!=(const subTime &, const subTime &);
	friend inline bool operator<(const subTime &, const subTime &);
	friend inline bool operator<=(const subTime &, const subTime &);
	friend inline bool operator>(const subTime &, const subTime &);
	friend inline bool operator>=(const subTime &, const subTime &);
	friend inline bool operator<(const subTime &, const int);
	friend inline bool operator>(const subTime &, const int);
	friend inline bool operator<=(const subTime &, const int);
	friend inline bool operator>=(const subTime &, const int);
	// friend string operator=(const subTime&);

	int h();
	int m();
	float s();
	float dur();

private:
	int hour{0}, minute{0};			  // integers for the hours n minutes
	float second{0.0}, duration{0.0}; // float for the seconds
};

// pairs should be sorted low to high
bool btwnIncl(subTime t, pair<subTime, subTime> p);
bool btwnUnIncl(subTime t, pair<subTime, subTime> p);
bool btwnIU(subTime t, pair<subTime, subTime> p);
bool btwnUI(subTime t, pair<subTime, subTime> p);

class cue
{
public:
	cue() {}
	cue(subTime s, subTime e, string &st, string &f, string &d);

	friend ostream &operator<<(ostream &os, const cue &);

	// friend class vector<cue>;
	subTime startTime{0.0}, endTime{0.0}; // has to be public for comparison reasons

protected:
	string fx{""}, style{""}, dialogue{""};
};


#endif