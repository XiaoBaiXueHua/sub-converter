#ifndef SUB_CONVERTER_OPTIONS
#define SUB_CONVERTER_OPTIONS
#include <iostream>
#include <iomanip>
#include <string>
#include <set>
#include <vector>
#include <fstream>
#include <sstream>
#include <regex>
#include <filesystem>
using namespace std;

class options
{
	// static class full of things
public:
	static filesystem::path directory, output; // directory for the subtitle files to be input
	static filesystem::path csv; // optional csv for telling us which files to convert. how it's processed depends on the child converters
	static bool csvMode, split, style, styleBorders;
};
// now we set the defaults
filesystem::path options::directory = "..";
filesystem::path options::output = "";
filesystem::path options::csv = "tracklist.csv";
bool options::csvMode = false;
bool options::split = false;
bool options::style = false;
bool options::styleBorders = false; // these tend to be really big chunks, so it's off by default

class fanmixOpts : options
{ // options for the fanmix stuff specifically
public:
	static string lyricStr, karaStr; // lyric n karaoke identification
	static filesystem::path lyrDir, commDir, htmlDir, comboDir; // folder names for the lyrics, commentary, and html directories
	static string mp3_host_url, mp3_host_ext, sub_host_url, sub_host_ext; // optional strings for systematically printing where the mp3s/subtitles are hosted for the html mode. these will always have to be manually configured in the config file
	static bool makeHTML, combine;
};
// set the defaults
string fanmixOpts::lyricStr = "Lyrics";
string fanmixOpts::karaStr = "kara";
filesystem::path fanmixOpts::lyrDir = "lyrics";
filesystem::path fanmixOpts::commDir = "comm";
filesystem::path fanmixOpts::htmlDir = "html";
filesystem::path fanmixOpts::comboDir = fanmixOpts::commDir; // default is the same as the comm
string fanmixOpts::mp3_host_url = "";
string fanmixOpts::mp3_host_ext = "";
string fanmixOpts::sub_host_url = "";
string fanmixOpts::sub_host_ext = "";
bool fanmixOpts::makeHTML = true; 
bool fanmixOpts::combine = false;

class utils {
	// utilities, basically
	public:
	static string tmp, tmp2;
	// static vector<string> lineInfo;
};
string utils::tmp = "";
string utils::tmp2 = ""; // initialize these
// vector<string> utils::lineInfo = {};

#endif