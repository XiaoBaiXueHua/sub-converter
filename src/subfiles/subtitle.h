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

string toLower(string str)
{
	string tmp{""};
	for (int i{0}; i < str.length(); i++)
	{
		char c{char(str[i])};
		tmp += tolower(c);
	}
	return tmp;
}

class subtitle
{
public:
	subtitle() {}; // default constructor
	subtitle(filesystem::path file)
	{
		n = toLower(pathify(file));													 // makes it lowercase, replace the spaces with "-", and remove everything other than letters, numbers, and dashes
		e = file.extension().string().substr(1, file.extension().string().length()); // cuts off the . at the start bc i think that's a bother
		p = file;
	}
	subtitle(vector<string> &csv) // this version of the constructor requires you to locate the path in the main
	{
		trackNum = csv[0];
		t = csv[1];
		a = csv[2];
		u = csv[3];
		n = toLower(pathify(t));
	}

	string name() { return n; }
	string ext() { return e; }
	filesystem::path path() { return p; }
	string title() { return t; }
	string track() { return trackNum; }
	string artist() { return a; }
	string url() { return u; }
	void setPath(filesystem::path pith) { p = pith; }
	void setPath(string pith) { p = filesystem::path(pith); }
	static string pathify(string str)
	{
		return regex_replace(regex_replace(str, regex("\\s+"), "-"), regex("'"), "");
	}
	static string pathify(filesystem::path file)
	{
		return regex_replace(regex_replace(file.stem().string(), regex("\\s+"), "-"), regex("'"), "");
	}

private:
	string e{"ass"}, n{""}, t{""}, a{""}, u{""}, trackNum{"01"}; // the filename, extension, and then the song title, artist, n url; default extension "ass"
	filesystem::path p{""};										 // the relative path
};

class subTime // this is basically just an elaborate float from a string. tbh.
{
public:
	subTime() {}; // default constructor
	subTime(string t)
	{

		regex nums("(\\d{1,2}):(\\d{2}):(\\d{2}\\.\\d{2})");
		// smatch smodge;
		sregex_iterator shoof{t.begin(), t.end(), nums};

		hour = stoi((*shoof)[1]); // they should always be like this
		minute = stoi((*shoof)[2]);
		second = stof((*shoof)[3]);
		// cout << "hour: " << hour << ", minute: " << minute << ", second: " << second << endl;

		duration = second + (minute * 60) + (hour * 60 * 60); // duration is in seconds, as a float
	}

	subTime(float f) // constructor from the float duration
	{
		hour = int(f / (60 * 60)); // gets the hours
		minute = int((f - (hour * 3600)) / 60);
		second = f - (minute * 60);
		duration = f;
	}
	// overload the operators
	friend ostream &operator<<(ostream &, const subTime &);
	// friend istream& operator>>(istream&, const subTime&);
	float operator+(const subTime &operand2) const // you are only being included so that operator- will not cry
	{
		return duration + operand2.duration;
	}
	float operator-(const subTime &operand2) const // we only need subtraction for this it'll be fine
	{
		return duration - operand2.duration;
	}
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

	int h() { return hour; }
	int m() { return minute; }
	float s() { return second; }
	float dur() { return duration; }

private:
	int hour{0}, minute{0};			  // integers for the hours n minutes
	float second{0.0}, duration{0.0}; // float for the seconds
};

// overload the operators
ostream &operator<<(ostream &os, const subTime &obj)
{
	os << fixed << setprecision(0) << setfill('0') << obj.hour << ":" << setw(2) << obj.minute << ":" << setw(6) << setprecision(3) << obj.second; // the 'setfill' pads out the numbers, n the setw sets the number of leading 0's
	return os;
}
inline bool operator==(const subTime &l, const subTime &r)
{
	return (l.duration == r.duration);
}
inline bool operator!=(const subTime &l, const subTime &r)
{
	return !(l == r);
}
inline bool operator<(const subTime &l, const subTime &r)
{
	return (l.duration < r.duration);
}
inline bool operator<=(const subTime &l, const subTime &r)
{
	return (l.duration <= r.duration);
};
inline bool operator>(const subTime &l, const subTime &r)
{
	return (r < l);
}
inline bool operator>=(const subTime &l, const subTime &r)
{
	return (r <= l);
};
inline bool operator<(const subTime &l, const int r)
{
	return (l.duration < r);
}
inline bool operator>(const subTime &l, const int r)
{
	return (l.duration > r);
}
inline bool operator<=(const subTime &l, const int r)
{
	return (l.duration <= r);
}
inline bool operator>=(const subTime &l, const int r)
{
	return (l.duration >= r);
}

// pairs should be sorted low to high
bool btwnIncl(subTime t, pair<subTime, subTime> p)
{
	return ((p.first <= t) && (t <= p.second));
}
bool btwnUnIncl(subTime t, pair<subTime, subTime> p)
{
	return ((p.first < t) && (t < p.second));
}
bool btwnIU(subTime t, pair<subTime, subTime> p)
{ // inclusive/uninclusive
	return ((p.first <= t) && (t < p.second));
}
bool btwnUI(subTime t, pair<subTime, subTime> p)
{ // uninclusive/inclusive
	return ((p.first < t) && (t <= p.second));
}

class cue
{
public:
	cue() {}
	cue(subTime s, subTime e, string &st, string &f, string &d)
	{
		// construct it from just the subTimes n string
		startTime = s;
		endTime = e;
		style = st;
		fx = f;
		dialogue = d;
	}

	friend ostream &operator<<(ostream &os, const cue &);

	// friend class vector<cue>;
	subTime startTime{0.0}, endTime{0.0}; // has to be public for comparison reasons

protected:
	string fx{""}, style{""}, dialogue{""};
};

ostream &operator<<(ostream &os, const cue &c)
{
	// we'll have to make an options/settings static class for this later but for now just whatever hardcode it
	string voice{c.fx};
	if (voice == "" && c.style == fanmixOpts::lyricStr) {
		voice = fanmixOpts::lyricStr;
	}
	if (voice != "" && voice != fanmixOpts::karaStr) {
		os << "<v " << voice << ">";
	}
	if (c.style == fanmixOpts::lyricStr)
	{
		os << "♪♫ (";
	}
	os << c.dialogue;
	if (c.style == fanmixOpts::lyricStr)
	{
		os << ") ♫♪";
	}
	if (voice != "" && voice != fanmixOpts::karaStr)
	{
		os << "</v>";
	}
	return os;
}

#endif