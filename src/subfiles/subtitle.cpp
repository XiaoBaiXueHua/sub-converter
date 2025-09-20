#include <string>
#include "subtitle.h"
#include "friends.h"

using namespace std;

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
			if (utils::tmp != "null")
			{
				c = (utils::tmp == "combined");
			}
			if (m > 5)
			{
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
	if (options::csvMode && (m > 4))
	{
		// csvMode overrides the static options
		fanmixOpts::combine = c;
	}
	p = pith;
}
void subtitle::setPath(string pith)
{
	if (options::csvMode && (m > 4))
	{
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

cue::cue(string l)
{
	if (regex_search(l, regex("^Comment:\\s+")))
	{
		// if it has "comment" at the start, then well. it's a comment lel
		cmt = true;
	}
	string t{""};
	int q{0}; // tracks how many comma-separated bits we've had so far
	stringstream stmp;
	vector<string> lineInfo = {};

	stmp << regex_replace(l, regex("^\\w+:\\w+"), ""); // get a tmp stringstream without the Comment/Dialogue: bit
	while (getline(stmp, t, ','))
	{
		// read as a csv

		if (q < 10) // ass specification will have this always be 10
		{
			// push into vector
			lineInfo.push_back(t);
			q++;
		} else {
			lineInfo[9] += "," + t; // concatenate dialogue lines with comments and add said commas back in
		}
	}
	while (lineInfo.size() < 10) {
		// in case we have empty cues for some reason
		lineInfo.push_back(" "); // add empty strings to it
	}

	// and save these things
	layer = stoi(lineInfo[0]);
	startTime = subTime(lineInfo[1]);
	endTime = subTime(lineInfo[2]);
	style = lineInfo[3]; 
	actor = lineInfo[4];
	marginL = stoi(lineInfo[5]);
	marginR = stoi(lineInfo[6]);
	marginV = stoi(lineInfo[7]);
	fx = lineInfo[8];
	dialogue = lineInfo[9];
}

cue::cue(subTime s, subTime e, string &st, string &f, string &d)
{
	// construct it from just the subTimes n string
	startTime = s;
	endTime = e;
	style = st;
	fx = f;
	dialogue = d;
}

bool cue::empty()
{
	return regex_search(dialogue, regex("^\\s*$")); // it's empty if it's nothing but spaces
}
bool cue::comment() { return cmt; }

void cue::sanitize()
{
	// so if it's not empty
	if (!empty())
	{
		// wretched regex learning: https://www.studyplan.dev/pro-cpp/regex-capture-groups
		smatch smidge;
		regex effects("\\{?\\\\(i|b|s|u)(\\d)\\}?");
		// first we make the <i>, <b>, and <s> tags
		while (regex_search(dialogue, smidge, effects))
		{
			utils::tmp = "\\\\" + smidge[1].str() + smidge[2].str(); // tmp2 you need a job. if it doesn't work, add like. four more backslashes i guess
			// miscSwitch = stoi(smidge[2].str());
			dialogue = regex_replace(dialogue, regex(utils::tmp), string("<" + string(stoi(smidge[2].str()) ? "" : "/") + smidge[1].str() + ">"));
		}

		// okay. what if the way we tried to replace the excess bracket stuff if like
		effects = regex("(\\{)(.*?)(\\})"); // you know what. just grisp the whole thing.
		while (regex_search(dialogue, smidge, effects))
		{
			// tmp2 = smidge[1].str(); // this represents the guts
			// seeMatches(smidge);
			stringstream steam(smidge[2].str());
			int len = smidge[0].str().length(); // length of the string

			// string tmp3{""};
			utils::tmp2 = ""; // reset this
			while (getline(steam, utils::tmp, '\\'))
			{
				// cout << utils::tmp << endl;
				smatch smush;
				if (regex_search(utils::tmp, smush, regex("(.*?)(<.*?>)(.*?)")))
				{
					// cout << "inline html: " << endl;
					// seeMatches(smush);
					utils::tmp2 += smush[2].str();
				}
			}
			// no regex necessary
			long long unsigned int s{dialogue.find(smidge[2].str())};
			dialogue = dialogue.replace(s - 1, len, utils::tmp2);
			break;
		}

		effects = regex("(\\{|\\})");
		// int r{0};
		while (regex_search(dialogue, smidge, effects))
		{
			// remove any leftover curly brackets
			dialogue = regex_replace(dialogue, effects, ""); // WHATEVER!!!! JUST GET RID OF CURLY BRACKETS UNIVERSALLY I GUESS!!!!!!!
		}

		effects = regex("\\\\N");
		while (regex_search(dialogue, smidge, effects))
		{
			dialogue = regex_replace(dialogue, effects, "\n"); // now it's just the raw dialogue line but with the newlines replaced
		}

		// this is producing some strange bugs around echoes n stuff in combined mode, most likely conflicting w/the operator override
		// if (fx != "" && !(fx == fanmixOpts::karaStr || fx == "comm")) // there's a bug here when doing batches for some reason OH it's bc we're processing the html earlier now i see i see
		// {
		// 	dialogue = "<v " + fx + ">" + dialogue + "</v>"; // for now, just give the voice whatever effect is going on there, as long as it's not blank or karaoke lol
		// }
	}
}

string cue::dial() { return dialogue; } // stopgap solution before i eventually fix up the lines vector to work with cues n stuff instead

string cue::effect() { return fx; } // gets the effect

string cue::s() { return style; } // gets the style

string cue::print() {
	// prints it ASS style
	stringstream os;
	os << cmt ? "Comment: " : "Dialogue: " << layer << "," << startTime << "," << endTime << "," << style << "," << actor << "," << marginL << "," << marginR << "," << marginV << "," << fx << "," << dialogue;
	// string d{regex_replace(dialogue, regex)}
	return os.str(); // hopefully this preserves backslashes. if not, then uhhhhh we'll have to do a replaceAll
}

// overloaded operator for cue
ostream &operator<<(ostream &os, const cue &c)
{
	// we'll have to make an options/settings static class for this later but for now just whatever hardcode it
	string voice{c.fx};
	if ((voice == "") && (c.style == fanmixOpts::lyricStr))  // only do these if we're like. Suppooooosed to
	// if ((voice == "") && (c.style == fanmixOpts::lyricStr) && cue::lyrics)  // only do these if we're like. Suppooooosed to
	{
		voice = fanmixOpts::lyricStr;
	}
	if ((voice != "") && (voice != fanmixOpts::karaStr))
	{
		os << "<v " << voice << ">";
	}
	if ((c.style == fanmixOpts::lyricStr))
	// if ((c.style == fanmixOpts::lyricStr) && cue::lyrics) // only do these if we're like. Suppooooosed to
	{
		os << "♪♫ (";
	}
	os << c.dialogue;
	if ((c.style == fanmixOpts::lyricStr))
	// if ((c.style == fanmixOpts::lyricStr) && cue::lyrics) // only do these if we're like. Suppooooosed to
	{
		os << ") ♫♪";
	}
	if ((voice != "") && (voice != fanmixOpts::karaStr))
	{
		os << "</v>";
	}
	return os;
}

inline bool operator==(const cue &l, const cue &r)
{
	return (l.dialogue == r.dialogue);
}
inline bool operator==(const cue &l, const string &r)
{
	return (l.dialogue == r);
}
inline bool operator==(const string &l, const cue &r)
{
	return (r == l);
}
inline bool operator!=(const cue &l, const cue &r)
{
	return !(l == r);
}
inline bool operator!=(const cue &l, const string &r)
{
	return !(l == r);
}
inline bool operator!=(const string &l, const cue &r)
{
	return !(r == l);
}