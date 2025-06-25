#include "options.h"
#include "friends.h"

using namespace std;

vector<string> meep = {};
fstream strm;
string tupper(string s)
{
	string u{""};
	for (const char c : s)
	{
		u += toupper(c);
	}
	return u;
}

bool tf(string s)
{
	if (tupper(s) == "TRUE" || s == "1")
	{
		return true;
	}
	else if (tupper(s) == "FALSE" || s == "0")
	{
		return false;
	}
	else
	{
		throw new logic_error("hey man. keywords true/false or 1/0 only.");
	}
}

// this is for parsing the ass colors and turning them into rgba
int htod(string c)
{ // takes strings of Exactly Two chars
	int first{0}, second{0};
	first = int(char(c[0])) - 48;
	second = int(char(c[1])) - 48;
	if (first > 10)
	{
		first = first - 8; // correct the ascii offset for the letters
	}
	if (second > 10)
	{
		second = second - 8; // correct the ascii offset for the letters
	}
	// cout << "first: " << first << "; second: " << second << ", (first * 16) + second = " << (first * 16) + second << endl;
	return (first * 16) + second;
}

string rgba(string c)
{
	float alpha = htod(c.substr(2, 2));
	int b{htod(c.substr(4, 2))}, g{htod(c.substr(6, 2))}, r{htod(c.substr(8, 2))};
	string color{"rgba(" + to_string(1 - (round(100 * alpha) / 100)).substr(0, 4) + "," + to_string(r) + "," + to_string(g) + "," + to_string(b) + ")"};
	return color;
}

void makeDir(string str)
{
	if (!filesystem::directory_entry(str).exists())
	{
		filesystem::create_directory(str);
	}
}
void makeDir(filesystem::path p)
{
	makeDir(p.string());
}
void config()
{
	meep.clear();
	strm.open("sub-converter.config");
	if (strm.is_open())
	{
		cout << "configuring..." << endl;
		// i = 1;
		stringstream stmp;
		while (getline(strm, utils::tmp)) // put each line into utils::tmp from the istream
		{
			// yay now we get to read the configuration file
			stmp.clear();

			// cout << "config line " << i << ": " << utils::tmp << endl;
			if (utils::tmp[0] != '#' && utils::tmp != "") // if it's neither a comment nor just blank
			{
				stmp << utils::tmp; // oh okay only write it to stmp after you're sure it's not a comment

				meep.clear();

				// cout << "\n\tline " << i << ".\ntmp[0]: " << utils::tmp[0] << "\ntmp2: " << utils::tmp2 << endl;

				while (getline(stmp, utils::tmp2, '=')) // key/values into utils::tmp2 via stmp
				{
					// cout << "utils::tmp2 during getline stmp: " << utils::tmp2 << endl;
					// i'm sure there's like a key-value pairing way of doing this
					meep.push_back(utils::tmp2);
				}

				// cout << meep[0] << ": " << meep[1] << endl;

				// i'm sure there's a faster way of doing this but i don't know it
				if (meep[0] == "csv")
				{
					// cout << "setting " << meep[0] << endl;
					options::csv = meep[1];
				}
				else if (meep[0] == "directory")
				{
					// cout << "setting " << meep[0] << endl;
					options::directory = meep[1];
				}
				else if (meep[0] == "sub-path")
				{
					// cout << "setting " << meep[0] << endl;
					options::output = meep[1];
				}
				else if (meep[0] == "splitter-mode")
				{
					options::split = tf(meep[1]);
				}
			}
			// i++; // increment i for debugging
		}
	}

	strm.close();
	strm.clear();

	// now, after all that's been configured, we should check to see if the relevant file folders exist
	// actually i'll figure that out later.
	makeDir(options::output);
	// makeDir(string(options::output+"/comm"));
	// makeDir(string(options::output+"/comm/ao3"));
	// makeDir(string(options::output+"/html"));
	// makeDir(string(options::output+"/lyrics"));
}

void configFanmix()
{
	// configuration options only for the fanmix
	meep.clear();
	strm.open("fanmix.config");
	if (strm.is_open())
	{
		cout << "configuring fanmix options..." << endl;
		stringstream stmp;
		while (getline(strm, utils::tmp)) // put each line into utils::tmp from the istream
		{
			// yay now we get to read the configuration file
			stmp.clear();

			// cout << "config line " << i << ": " << utils::tmp << endl;
			if (utils::tmp[0] != '#' && utils::tmp != "") // if it's neither a comment nor just blank
			{
				stmp << utils::tmp; // oh okay only write it to stmp after you're sure it's not a comment

				meep.clear();

				// cout << "\n\tline " << i << ".\ntmp[0]: " << utils::tmp[0] << "\ntmp2: " << utils::tmp2 << endl;

				while (getline(stmp, utils::tmp2, '=')) // key/values into utils::tmp2 via stmp
				{
					// cout << "utils::tmp2 during getline stmp: " << utils::tmp2 << endl;
					// i'm sure there's like a key-value pairing way of doing this
					meep.push_back(utils::tmp2);
				}

				// cout << meep[0] << ": " << meep[1] << endl;

				// i'm sure there's a faster way of doing this but i don't know it
				// you can override some of the default options configured in sub-converter.config
				if (meep[0] == "csv")
				{
					// cout << "setting " << meep[0] << endl;
					options::csv = meep[1];
				}
				else if (meep[0] == "directory")
				{
					// cout << "setting " << meep[0] << endl;
					options::directory = meep[1];
				}
				else if (meep[0] == "sub-path")
				{
					// cout << "setting " << meep[0] << endl;
					options::output = meep[1];
				}
				else if (meep[0] == "mp3-host-url")
				{
					// cout << "setting " << meep[0] << endl;
					fanmixOpts::mp3_host_url = meep[1];
				}
				else if (meep[0] == "mp3-host-ext")
				{
					// cout << "setting " << meep[0] << endl;
					fanmixOpts::mp3_host_ext = meep[1];
				}
				else if (meep[0] == "sub-host-url")
				{
					// cout << "setting " << meep[0] << endl;
					fanmixOpts::sub_host_url = meep[1];
				}
				else if (meep[0] == "sub-host-ext")
				{
					// cout << "setting " << meep[0] << endl;
					fanmixOpts::sub_host_ext = meep[1];
				}
				else if (meep[0] == "lyric-style")
				{
					// cout << "setting " << meep[0] << endl;
					fanmixOpts::lyricStr = meep[1];
				}
				else if (meep[0] == "kara-fx")
				{
					// cout << "setting " << meep[0] << endl;
					fanmixOpts::karaStr = meep[1];
				}
				else if (meep[0] == "lyric-dir")
				{
					fanmixOpts::lyrDir = meep[1];
				}
				else if (meep[0] == "comm-dir")
				{
					fanmixOpts::commDir = meep[1];
				}
				else if (meep[0] == "combine-dir")
				{
					fanmixOpts::comboDir = meep[1];
				}
				else if (meep[0] == "combined-track")
				{
					fanmixOpts::combine = tf(meep[1]);
				}
				else if (meep[0] == "make-html")
				{
					fanmixOpts::makeHTML = tf(meep[1]);
				}
			}
			// i++; // increment i for debugging
		}
	}
	strm.close();
	strm.clear();

	// make the configured directories
	// this is smth that should really only be done in the splitter mode, but ignore that for now
	// that said, could probably make a static options list for directory outputs or smth
	makeDir(options::output.string() + "/" + fanmixOpts::lyrDir.string());
	makeDir(options::output.string() + "/" + fanmixOpts::commDir.string());
	makeDir(options::output.string() + "/" + fanmixOpts::htmlDir.string());
	if (fanmixOpts::combine) {
		makeDir(options::output.string() + "/" + fanmixOpts::comboDir.string());
	}
}


string outStrings(string folder, subtitle sub, string appendum, string prependum) // appendum then prependum bc there's more likely to be smth appended
{
	string t{""};
	if (options::output.string() != "")
	{
		// if we have a proper output folder
		t += options::output.string() + "/";
	}

	if (folder != "") {
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
//