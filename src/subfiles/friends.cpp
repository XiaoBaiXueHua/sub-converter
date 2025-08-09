#include <iostream>
#include <iomanip>
#include <filesystem>
#include <vector>
#include <regex>
#include "options.h"
#include "subtitle.h"
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
				else if (meep[0] == "output")
				{
					// cout << "setting " << meep[0] << endl;
					options::output = meep[1];
				}
				else if (meep[0] == "splitter-mode")
				{
					options::split = tf(meep[1]);
				}
				else if (meep[0] == "stylesheet")
				{
					options::style = tf(meep[1]);
				}
				else if (meep[0] == "style-borders")
				{
					options::styleBorders = tf(meep[1]);
				}
				else if (meep[0] == "filetype")
				{
					utils::tmp2 = meep[1];
					if (utils::tmp2 != "ass" || utils::tmp2 != "srt" || utils::tmp2 != "all")
					{ // only accept ass, srt, or both
						options::filetype = utils::tmp2;
					};
				}
				else if (meep[0] == "keep-gaps")
				{
					options::gaps = tf(meep[1]);
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
				else if (meep[0] == "output")
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
				else if (meep[0] == "html-dir") {
					fanmixOpts::htmlDir = meep[1];
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
				else if (meep[0] == "keep-gaps")
				{
					options::gaps = tf(meep[1]);
				}
			}
			// i++; // increment i for debugging
		}
	}
	strm.close();
	strm.clear();

	options::combine = fanmixOpts::combine; // keep this safe

	// make the configured directories
	// this is smth that should really only be done in the splitter mode, but ignore that for now
	// that said, could probably make a static options list for directory outputs or smth
	makeDir(options::output.string() + "/" + fanmixOpts::lyrDir.string());
	makeDir(options::output.string() + "/" + fanmixOpts::commDir.string());
	makeDir(options::output.string() + "/" + fanmixOpts::htmlDir.string());
	if (fanmixOpts::combine)
	{
		makeDir(options::output.string() + "/" + fanmixOpts::comboDir.string());
	}
}

void seeMatches(smatch s)
{
	int i{0};
	for (auto &m : s)
	{
		cout << i << ". " << m.str() << endl;
		i++;
	}
	cout << endl;
}

filesystem::path locator(string fname, string ext = options::filetype)
{ // default look for an .ass file
	bool found{false};
	filesystem::path tmpath{""};
	for (auto const &dir_entry : filesystem::recursive_directory_iterator{filesystem::directory_entry{options::directory}})
	{
		if (!dir_entry.is_directory())
		{
			if ((toLower(dir_entry.path().stem().string()) == toLower(fname)) && (toLower(dir_entry.path().extension().string()) == string("." + toLower(ext))))
			{ // make sure they're both lowercase. just for comparison lol
				found = true;
				tmpath = dir_entry.path();
				cout << "found file: " << tmpath << endl;
				break; // and also snap out of it lol
			}
		}
	}

	if (found)
	{
		return tmpath;
	}
	else
	{
		throw new exception;
	}
}

void quickOpen(fstream &stream, string path)
{
	stream.open(path);
	if (!stream.is_open())
	{
		stream.clear();
		stream.open(path, ios::out); // makes the file if it's not already open
		stream.close();
		stream.open(path);
	}
	cout << "opened " << path << " via string.\n";
	// quickOpen(stream, path);
}
// overloading for path object types
void quickOpen(fstream &stream, filesystem::path path)
{
	stream.open(path);
	if (!stream.is_open())
	{
		stream.clear();
		stream.open(path, ios::out); // makes the file if it's not already open
		stream.close();
		stream.open(path);
	}
	cout << "opened " << path << " via path.\n";
}


void showFiles(vector<filesystem::path> &paths, string ext) {
	if (toLower(ext) == "all")
	{
		showFiles(paths);
	}
	else
	{
		int i{1}; // set this to 1
		cout << setw(15) << 0 << ".\tAll" << endl;
		paths.clear(); // clears the vector of available files
		for (auto const &dir_entry : filesystem::recursive_directory_iterator{filesystem::directory_entry{options::directory}})
		{
			if (!dir_entry.is_directory())
			{
				filesystem::path currPath{dir_entry.path()}; // turn it into a path object
				// cout << "File type " << currPath.extension() << endl;
				if (currPath.extension() == string("." + ext))
				{
					if (currPath.has_parent_path())
					{
						if (!regex_search(currPath.parent_path().string(), regex("drafts"))) // ignore any drafts
						{
							cout << setw(15) << i << ". " << currPath << endl;
							paths.push_back(currPath);
							i++;
						}
					}
				}
			}
		}
	}
}

void showFiles(vector<filesystem::path> &paths) {
	//
}

//