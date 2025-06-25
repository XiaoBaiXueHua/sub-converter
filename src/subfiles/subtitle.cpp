#include <string>
#include "subtitle.h"
#include "friends.h"

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

/* subtitle */
subtitle::subtitle(filesystem::path file)
{
	n = toLower(pathify(file));													 // makes it lowercase, replace the spaces with "-", and remove everything other than letters, numbers, and dashes
	e = file.extension().string().substr(1, file.extension().string().length()); // cuts off the . at the start bc i think that's a bother
	p = file;
}
subtitle::subtitle(vector<string> &csv) // this version of the constructor requires you to locate the path in the main
{
	m = csv.size();
	trackNum = csv[0];
	t = csv[1];
	a = csv[2];
	u = csv[3];
	n = toLower(pathify(t));
	// those are the mandatory csv values
	if (m > 3)
	{
		// do something with all the alternate urls... probably separated by spaces?
		if (m > 4)
		{
			// lets us know if we're going to have a separate combined file
			utils::tmp = toLower(csv[5]);
			// c = (toLower(csv[5]) == "combined");
			if (utils::tmp != "null") {
				c = (utils::tmp == "combined");
			}
			if (m > 5) {
				// then we're getting into setting the extension
				e = csv[6];
			}
		}
	}
}

string subtitle::name() { return n; }
string subtitle::ext() { return e; }
filesystem::path subtitle::path() { return p; }
string subtitle::title() { return t; }
string subtitle::track() { return trackNum; }
string subtitle::artist() { return a; }
string subtitle::url() { return u; }
bool subtitle::combined() { return c; }
void subtitle::setPath(filesystem::path pith)
{
	if (options::csvMode && (m > 4)) {
		// csvMode overrides the static options
		fanmixOpts::combine = c;
	}
	p = pith;
}
void subtitle::setPath(string pith)
{
	if (options::csvMode && (m > 4)) {
		// csvMode overrides the static options
		fanmixOpts::combine = c;
	}
	p = filesystem::path(pith);
}
string subtitle::pathify(string str)
{
	return regex_replace(regex_replace(str, regex("\\s+"), "-"), regex("'"), "");
}
string subtitle::pathify(filesystem::path file)
{
	return regex_replace(regex_replace(file.stem().string(), regex("\\s+"), "-"), regex("'"), "");
}

/* subTime */
subTime::subTime(string t)
{

	regex nums("(\\d{1,2}):(\\d{2}):(\\d{2}(\\.|,)\\d{2,3})");
	// smatch smodge;
	sregex_iterator shoof{t.begin(), t.end(), nums};

	hour = stoi((*shoof)[1]); // they should always be like this
	minute = stoi((*shoof)[2]);
	// second = stof(string((*shoof)[3]).replace(2, 1, "."));
	// cout <<
	string tmps = (*shoof)[3];
	// cout << tmps << endl;
	if (options::filetype == "srt")
	{
		tmps = tmps.replace(2, 1, "."); // srt files get their commas replaced
	}
	second = stof(tmps);
	// cout << "hour: " << hour << ", minute: " << minute << ", second: " << second << endl;

	duration = second + (minute * 60) + (hour * 60 * 60); // duration is in seconds, as a float
}

subTime::subTime(float f) // constructor from the float duration
{
	hour = int(f / (60 * 60)); // gets the hours
	minute = int((f - (hour * 3600)) / 60);
	second = f - (minute * 60);
	duration = f;
}
int subTime::h() { return hour; }
int subTime::m() { return minute; }
float subTime::s() { return second; }
float subTime::dur() { return duration; }

// overload the operators
float subTime::operator+(const subTime &operand2) const // you are only being included so that operator- will not cry
{
	return duration + operand2.duration;
}
float subTime::operator-(const subTime &operand2) const // we only need subtraction for this it'll be fine
{
	return duration - operand2.duration;
}
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

/* btwn functions */
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

/* cue */

cue::cue(subTime s, subTime e, string &st, string &f, string &d)
{
	// construct it from just the subTimes n string
	startTime = s;
	endTime = e;
	style = st;
	fx = f;
	dialogue = d;
}
// overloaded operator for cue
ostream &operator<<(ostream &os, const cue &c)
{
	// we'll have to make an options/settings static class for this later but for now just whatever hardcode it
	string voice{c.fx};
	if (voice == "" && c.style == fanmixOpts::lyricStr)
	{
		voice = fanmixOpts::lyricStr;
	}
	if (voice != "" && voice != fanmixOpts::karaStr)
	{
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

inline bool operator==(const cue &l, const cue &r) {
	return (l.dialogue == r.dialogue);
}
inline bool operator==(const cue &l, const string &r) {
	return (l.dialogue == r);
}
inline bool operator==(const string &l, const cue &r) {
	return (r == l);
}
inline bool operator!=(const cue &l, const cue &r) {
	return !(l == r);
}
inline bool operator!=(const cue&l, const string &r) {
	return !(l == r);
}
inline bool operator!=(const string &l, const cue &r) {
	return !(r == l);
}