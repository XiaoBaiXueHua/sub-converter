#include <iostream>
#include <iomanip>
#include <string>
#include <set>
#include <vector>
#include <fstream>
#include <sstream>
#include <regex>
#include <filesystem>
#include "./subtitle.h"

using namespace std;

string tmp{""}, tmp2{""}; // my beloved, versatile tmps for everything
stringstream stmp{""};

// default paths n values n whatnot for the configuration
filesystem::path subfile{"tracklist.csv"}, directory{".."};
string lyricStr{"Lyrics"}, karaStr{"kara"}, mp3_host_url{"https://file.garden/ZxXPG2A7WCEI6W2A"}, mp3_host_ext{""}, sub_host_url{"https://sincerelyandyourstruly.neocities.org"}, sub_host_ext{"subs"}, subpath{"subspath"};

fstream istr, ostr, cstr, hstr; // io file streams, w/cstr being for the commentary file & hstr being the html file for ao3 later
smatch smidge;					// for when we're matching things

vector<filesystem::path> available;
vector<vector<string>> lines{}, display{}; // a vector for all the lines, so that later it can be iterated through and examined for timing things
vector<string> lineInfo{};				   // a vector for strings

pair<subTime, subTime> timestamps;
cue freminet; // so. it's the pair of timestamps, the style, the effects, and the line string
vector<cue> lyney, lynette;

int i{1}, j{0}, k{0}; // our beloved three random job integers
int opt{0};
bool csvMode{false}, miscSwitch{false};

// function loop for showing the available ass files
void showFiles(string ext);
int csvInput(fstream &stream, string fname);
filesystem::path locator(string fname, string ext);
void quickOpen(fstream &stream, string path);
void quickOpen(fstream &stream, filesystem::path path);
void ASStoVT(subtitle);
void config(); // configuration function
void printLine(vector<cue> &, subTime, subTime, int &);

int main()
{
	config(); // read the config file

	k = csvInput(ostr, subfile.string()); // k gets used for karaoke too, yeah, but it doesn't get used again in the main function, so we can transform it as we please in the other files
	// int hi = 1;

	if (csvMode)
	{
		if (k == 0)
		{
			// if batch is chosen
			for (auto &f : display)
			{
				subtitle soup(f);
				try
				{
					cout << "now searching for: " << soup.title() << endl;
					subfile = locator(toLower(soup.title()), "ass");
					soup.setPath(subfile);
					ASStoVT(soup);
				}
				catch (exception)
				{
					cout << "whoops! couldn't find the file for " << soup.title() << " :<" << endl;
				}
			}
		}
		else
		{
			lineInfo = display[k - 1]; // this has the title, artist, and youtube link now
			subtitle soup{lineInfo};
			tmp = toLower(soup.title()); // find the title
			cout << "now searching for: " << soup.title() << endl;
			try
			{
				subfile = locator(tmp, "ass");
				soup.setPath(subfile);
				ASStoVT(soup);
			}
			catch (exception)
			{
				cout << "whoops! couldn't find that file. :(" << endl;
				exit(69);
			}
		}
	}
	else
	{
		//
		if (k == 0)
		{
			// if batch is chosen
			for (auto &f : available)
			{
				ASStoVT(subtitle(f)); // then loop through everything
			}
		}
		else
		{
			ASStoVT(subtitle(available[k - 1])); // otherwise, only convert the one chosen
		}
	}

	cout << "Press any key to close. ";
	cin >> tmp;

	return 0;
}

void config()
{

	istr.open("sub-converter.config");
	if (istr.is_open())
	{
		cout << "configuring..." << endl;
		// i = 1;
		while (getline(istr, tmp)) // put each line into tmp from the istream
		{
			// yay now we get to read the configuration file
			stmp.clear();

			// cout << "config line " << i << ": " << tmp << endl;
			if (tmp[0] != '#' && tmp != "") // if it's neither a comment nor just blank
			{
				stmp << tmp; // oh okay only write it to stmp after you're sure it's not a comment

				lineInfo.clear();

				// cout << "\n\tline " << i << ".\ntmp[0]: " << tmp[0] << "\ntmp2: " << tmp2 << endl;

				while (getline(stmp, tmp2, '=')) // key/values into tmp2 via stmp
				{
					// cout << "tmp2 during getline stmp: " << tmp2 << endl;
					// i'm sure there's like a key-value pairing way of doing this
					lineInfo.push_back(tmp2);
				}

				// cout << lineInfo[0] << ": " << lineInfo[1] << endl;

				// i'm sure there's a faster way of doing this but i don't know it
				if (lineInfo[0] == "csv")
				{
					// cout << "setting " << lineInfo[0] << endl;
					subfile = lineInfo[1];
				}
				else if (lineInfo[0] == "directory")
				{
					// cout << "setting " << lineInfo[0] << endl;
					directory = lineInfo[1];
				}
				else if (lineInfo[0] == "sub-path")
				{
					// cout << "setting " << lineInfo[0] << endl;
					subpath = lineInfo[1];
				}
				else if (lineInfo[0] == "mp3-host-url")
				{
					// cout << "setting " << lineInfo[0] << endl;
					mp3_host_url = lineInfo[1];
				}
				else if (lineInfo[0] == "mp3-host-ext")
				{
					// cout << "setting " << lineInfo[0] << endl;
					mp3_host_ext = lineInfo[1];
				}
				else if (lineInfo[0] == "sub-host-url")
				{
					// cout << "setting " << lineInfo[0] << endl;
					sub_host_url = lineInfo[1];
				}
				else if (lineInfo[0] == "sub-host-ext")
				{
					// cout << "setting " << lineInfo[0] << endl;
					sub_host_ext = lineInfo[1];
				}
				else if (lineInfo[0] == "lyric-style")
				{
					// cout << "setting " << lineInfo[0] << endl;
					lyricStr = lineInfo[1];
				}
				else if (lineInfo[0] == "kara-fx")
				{
					// cout << "setting " << lineInfo[0] << endl;
					karaStr = lineInfo[1];
				}
			}
			// i++; // increment i for debugging
		}
	}

	istr.close();
	istr.clear();

	// now, after all that's been configured, we should check to see if the relevant file folders exist
	// actually i'll figure that out later.
	// makeDir(subpath);
	// makeDir(string(subpath+"/comm"));
	// makeDir(string(subpath+"/comm/ao3"));
	// makeDir(string(subpath+"/html"));
	// makeDir(string(subpath+"/lyrics"));
}

filesystem::path locator(string fname, string ext = "ass")
{ // default look for an .ass file
	bool found{false};
	filesystem::path tmpath{""};
	for (auto const &dir_entry : filesystem::recursive_directory_iterator{filesystem::directory_entry{directory}})
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
	// cout << "opening " << path << "..." << endl;
	if (!stream.is_open())
	{
		stream.clear();
		stream.open(path, ios::out | ios::trunc); // makes the file if it's not already open
		stream.close();
		stream.open(path);
	}
	// cout << "opened " << path << " via string.\n";
}
// overloading for path object types
void quickOpen(fstream &stream, filesystem::path path)
{
	stream.open(path);
	// cout << "opening " << path << "..." << endl;
	if (!stream.is_open())
	{
		stream.clear();
		stream.open(path, ios::out | ios::trunc); // makes the file if it's not already open
		stream.close();
		stream.open(path);
	}
	// cout << "opened " << path << " via path.\n";
}

int csvInput(fstream &stream, string fname)
{
	csvMode = false;
	stream.open(fname);
	// j = -1;

	if (!stream.is_open())
	{
		cout << "There doesn't seem to be a file by the name of " << fname << ". Would you like to:\n\t1. try to locate a csv file, or\n\t2. just look at all the subtitle files available? ";
		cin >> opt;
		switch (opt)
		{
		case 1:
		{
			showFiles("csv");
			if (available.size() > 0)
			{
				miscSwitch = true;
				cout << "Please select your csv file: ";
				cin >> opt;
				while (opt < 0 || opt > available.size())
				{
					cout << "uhh yeah you can't actually do that here. try again: ";
					cin >> opt;
				}
				cout << "um. i haven't gotten that far yet actually. sowwie. go fix your config file.";
				exit(0);
				csvMode = true; // only do this if there are actually csv files available
								// j = csvInput(stream, available[opt - 1].string()); // uhhh then in order to make this recursive we set k to smth
			}
			else
			{
				cout << "damn. you hae NO csv files. pick an ass file instead: ";
				showFiles("ass");
				cout << "> ";
				cin >> opt;
			}
			break;
		}
		case 2:
		{
			cout << "\nFiles available for conversion:" << endl;
			showFiles("ass");
			break;
		}
		default:
		{
			cout << "hi. you can't do that. gonna show you all the existing ass files instead: " << endl;
			// choice = 2; // set this for y'know, The Thing later
			cout << "\nFiles available for conversion:" << endl;
			showFiles("ass");
		}
		}
	}
	else
	{
		// otherwise yeah it does exist so get to processing
		csvMode = true;
		// choice = 1; // turn on the "we're doing this the csv route" switch
		cout << "Listed Tracks: " << endl;
		cout << setw(7) << 0 << ". (All)" << endl;
		display.clear();	  // gonna use this to hold stuff
		getline(stream, tmp); // skip past the first title line
		j = 1;
		while (getline(stream, tmp))
		{
			lineInfo.clear();
			stmp.clear(); // haha. don't forget to clear this thing lol
			stmp << tmp;  // feed that into a string stream
			i = 0;
			while (getline(stmp, tmp2, ','))
			{
				// cout << tmp2 << "\t";
				if (i == 0)
				{
					lineInfo.push_back(string((j < 10) ? string("0" + to_string(j)) : to_string(j)));
					cout << setw(7) << j << ". " << tmp2 << endl;
					j++;
				}
				lineInfo.push_back(tmp2);
				i++;
			}
			display.push_back(lineInfo);
		}
	}

	cout << "\n\tPlease select your file: ";
	cin >> opt;
	if (csvMode)
	{
		// csv mode means we have to look through display instead
		while (opt > display.size() || opt < 0)
		{
			cout << "hey you can't do that. choose another number: ";
			cin >> opt;
		}
	}
	else
	{
		while (opt > available.size() || opt < 0)
		{
			cout << "hey you can't do that. choose another number: ";
			cin >> opt;
		}
	}
	
	stream.close(); // and then close it now that we're done
	stream.clear(); // and probably clear it for good measure
	// return {choice, opt};
	return opt;
}

void showFiles(string ext = "ass") // default are .ass files
{
	i = 1; // set this to 1
	cout << setw(15) << 0 << ".\tAll" << endl;
	available.clear(); // clears the vector of available files
	for (auto const &dir_entry : filesystem::recursive_directory_iterator{filesystem::directory_entry{directory}})
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
						available.push_back(currPath);
						i++;
					}
				}
			}
		}
	}
}

void ASStoVT(subtitle ass)
{
	vector<string> htmlClasses{"cue", "timestamp"}; // the html classes vector has the cue divs and timestamp spans
	lineInfo.clear();								// clear this out of whatever it had before
	// freminet.clear();

	cout << "Now working with " << ass.name() << "." << ass.ext() << endl;
	quickOpen(istr, ass.path()); // open the relevant subtitle file

	quickOpen(ostr, string(directory.string() + (subpath != "" ? "/" : "") + subpath + "/lyrics/" + ass.name() + ".vtt"));											// make the output file
	quickOpen(cstr, string(directory.string() + (subpath != "" ? "/" : "") + subpath + "/comm/" + ass.name() + "-comm.vtt"));										// make the commentary track
	quickOpen(hstr, string(directory.string() + (subpath != "" ? "/" : "") + subpath + "/html/" + (csvMode ? ass.track() + "-" : "") + ass.name() + "-comm.html")); // make the html file

	ostr.clear();
	cstr.clear(); // clear the output streams before we start writing
	hstr.clear();

	ostr << "WEBVTT\n\n\n"; // write this at the top
	cstr << "WEBVTT\n\n\n";

	hstr << "<details class=\"annotations-controller\" open><summary>&nbsp;</summary></details>" << endl; // make the annotations controller

	hstr << "<video width=\"100%\" controls=\"controls\" controlslist=\"nodownload\" playsinline=\"playsinline\" crossorigin=\"anonymous\" preload=\"metadata\">";

	if (csvMode)
	{
		// if we're in csv mode, then we can also write the fallback youtube video
		hstr << "\n\thi. it appears that your browser does not support playing video in html5. wild, considering this day and age. <br />however, you can still listen to this song <a href=\"" << ass.url() << "\">on youtube</a>!";
	}
	hstr << "\n\t<source src=\"" << mp3_host_url << (mp3_host_ext != "" ? "/" : "") << mp3_host_ext << "/"; // now we start writing the source
	if (csvMode)
	{
		// if we're in csv mode, then that means we were granted Important Info which we can then use
		hstr << ass.track() << "-" << subtitle::pathify(ass.artist()) << "-" << subtitle::pathify(ass.title());
	}
	else
	{
		hstr << "[HI. DON'T FORGET TO FILL THIS OUT WITH NUMBER-ARTIST-TITLE]";
	}
	hstr << ".mp3\" type=\"audio/mp3\">";

	hstr << "\n\t<track srclang=\"en\" label=\"Annotations\" kind=\"subtitles\" src=\"" << sub_host_url << (sub_host_ext != "" ? "/" : "") << sub_host_ext << "/comm/" << ass.name() << "-comm.vtt\" default=\"default\">\n</video>" << endl; // open up the html file with this and write the boilerplate for the html"

	hstr << "<hr />" << endl; // and you know what. we deserve a horizontal line.

	hstr << "<details>\n\t<summary><h3 class=\"file-declaration\">WEBVTT</h3></summary>" << endl; // details n summary
	hstr << "<p></p>" << endl;																	  // extra gap
	hstr << "<div class=\"scrolly\">" << endl;													  // div for scrolling through the cues

	// move it along to the events
	if (tmp != "[Events]")
	{
		while (getline(istr, tmp))
		{
			if (tmp == "[Events]")
			{
				break;
			}
		}
	}

	getline(istr, tmp); // now we get the line After, which is just the "Format: whatever,whatever,whatever" thing. which we don't need here, so the lines after have been removed

	lines.clear();											// clear this out of whatever it was used for last
	const int formats = 10;									// ASS specification will have this always be 10
	subTime startTime{"0:00:00.00"}, endTime{"0:00:00.00"}; // get these from the cue-gap comments

	miscSwitch = true; // thinking abt it, could probably reuse this elsewhere, since it only gets used that one time
	bool lyricLine{false}, moreLines{false}, kara{false};
	string fx{""};

	// turns all the lines into a vector of strings
	while (getline(istr, tmp))
	{
		// now we get to processing the actual meat of it
		lineInfo.clear();
		stmp.clear();

		i = 0; // tracks how many comma-separated things we've parsed so far so that we know when to start concatenating the dialogue strings

		stmp << regex_replace(tmp, regex("^\\w+:\\s+"), ""); // keep tmp as it is for the later regex_search and just write the version w/the "Comment/Dialogue:" bit cleared off straight to the string stream

		while (getline(stmp, tmp2, ',')) // process this regardless of whether it's a comment or not
		{
			if (i < formats)
			{
				lineInfo.push_back(tmp2);
				i++;
			}
			else
			{
				lineInfo[formats - 1] += "," + tmp2; // concatenate the dialogue lines even when they have commas :D ...and also add the commas back in lol
			}
		}
		while (lineInfo.size() < formats)
		{
			lineInfo.push_back(" "); // add an empty string to it if there's nothing in that line. this was mostly for when empty dialogue lines were used as gaps
		}
		// okay now done making the lineInfos
		lyricLine = (lineInfo[3] == lyricStr), kara = (fx == karaStr);

		if (lineInfo[8] == "cue-gap") // if the effect is "cue-gap", then save that info
		{
			(miscSwitch ? startTime : endTime) = lineInfo[(miscSwitch ? 1 : 2)];
			miscSwitch = false; // switch this off lol
								// cout << "startTime: " << startTime << " endTime: " << endTime << endl;
		}
		else if (!regex_search(tmp, regex("^Comment:\\s+"))) // ignore comments for now, though .vtt supports them
		{

			// process the html before we pass it along
			tmp = lineInfo[9]; // all right time to start working with the subtitle line itself
			// if we're not working with a totally empty line
			if (tmp != " ")
			{
				// wretched regex learning: https://www.studyplan.dev/pro-cpp/regex-capture-groups
				regex effects("\\{?\\\\(i|b|s|u)(\\d)\\}?");
				// first we make the <i>, <b>, and <s> tags
				while (regex_search(tmp, smidge, effects))
				{
					tmp2 = "\\\\" + smidge[1].str() + smidge[2].str(); // tmp2 you need a job. if it doesn't work, add like. four more backslashes i guess
					miscSwitch = stoi(smidge[2].str());
					tmp = regex_replace(tmp, regex(tmp2), string("<" + string(miscSwitch ? "" : "/") + smidge[1].str() + ">"));
				}

				effects = regex("(\\{|\\})");
				while (regex_search(tmp, smidge, effects))
				{
					tmp = regex_replace(tmp, effects, ""); // WHATEVER!!!! JUST GET RID OF CURLY BRACKETS UNIVERSALLY I GUESS!!!!!!!
				}

				effects = regex("\\\\N");
				while (regex_search(tmp, smidge, effects))
				{
					tmp = regex_replace(tmp, effects, "\n"); // now it's just the raw dialogue line but with the newlines replaced
				}

				if (fx != "" && (fx != karaStr || fx != "comm")) // there's a bug here when doing batches for some reason OH it's bc we're processing the html earlier now i see i see
				{
					tmp = "<v " + fx + ">" + tmp + "</v>"; // for now, just give the voice whatever effect is going on there, as long as it's not blank or karaoke lol
				}
			}
			lineInfo[9] = tmp;
			// freminet = cue(make_tuple(make_pair(subTime(lineInfo[1]), subTime(lineInfo[2])), lineInfo[3], lineInfo[8], lineInfo[9])); // the timestamps, the style, the effect, and then the line string

			freminet = cue(subTime(lineInfo[1]), subTime(lineInfo[2]), lineInfo[3], lineInfo[8], lineInfo[9]);
			lineInfo[3] == lyricStr ? lyney.push_back(freminet) : lynette.push_back(freminet); // lyney has lyrics; lynette has annotations

			lines.push_back(lineInfo); // add the line to the vector
		}
	}

	// now we loop through that 2d vector
	i = 1; // set this to 1 as we use it to form ids for each cue
	j = 0; // j tracks the gaps
	k = 0; // k tracks the karaoke

	bool karaMode{false};		 // a boolean to track if we're in karaoke mode
	vector<string> prevLine{""}; // ugh but. i guess it seems i need another vector for the previous line too!!!
	set<subTime> times{};
	subTime startCue, endCue;

	for (int line{0}; line < lines.size(); line++)
	{
		// cout << "line " << line << endl;
		tmp = "";				// clear this out
		lineInfo = lines[line]; // i'm not setting aside more memory for a new vector. why would i do that.
		fx = lineInfo[8];
		// string fx{lineInfo[8]};		 // we'll always be working with these, so we can just define them i guess
		startCue = lineInfo[1], endCue = lineInfo[2];
		// um. don't forget to fill out the times set lol
		times.insert(startCue);
		times.insert(endCue);
		lyricLine = (lineInfo[3] == lyricStr), moreLines = (line < lines.size() - 1), kara = (fx == karaStr);

		if (line > 0) // only do this when i > 1 is to make sure we're not forming some sort of gap btwn the last lyric line and the first commentary line lol
		{

			prevLine = lines[line - 1];	   // if we can do this...
			string prevStyle{prevLine[3]}; // then we should also be able to get this info as well
			subTime prevEnd{prevLine[2]};
			// bool ;

			if (prevEnd != startCue || (prevStyle == lyricStr && startCue >= 0)) // if we're working with non-continuous subs OR we go from the lyrics to the annotations
			{
				// if ((prevStyle == "Lyrics" && !lyricLine && startCue >= 0)) // if we're not on a lyric line, but the previous line WAS a lyric line, AND if the start cue is greater than 0, then that means we're doing the end/start gaps for the different files
				if ((prevStyle == lyricStr && !lyricLine && startCue >= 0)) // if we're not on a lyric line, but the previous line WAS a lyric line, AND if the start cue is greater than 0, then that means we're doing the end/start gaps for the different files
				{

					prevEnd = startTime; // then reset the prev end time to be the same as the start time, for later cues and gapping
				}

				if (prevEnd < startCue) // regular gaps
				{

					if (!lyricLine) // commentary tracks get the cue ids
					{
						cstr << "gap-" << j << endl;					// add in an identifier for gaps for the non-lyric lines
						cstr << prevEnd << " --> " << startCue << endl; // otherwise, still write in the gaps, but only to the commentary lines
						cstr << " " << endl
							 << endl;

						hstr << "<div class=\"" << htmlClasses[0] << "\"><h4>gap-" << j << "<br /><span class=\"" << htmlClasses[1] << "\">" << prevEnd << " --&gt; " << startCue << "</span></h4>\n<p>&nbsp;</p></div>" << endl; // write the gaps to the thing too // actually. i just realized we don't actually need the gaps in here.

						j++; // also only increment j when actually Writing the gap identifiers lol
					}
				}
			}
		}
		// annotations lines should only be past the first line anyway, but anyway
		if (!lyricLine) // only cue up the commentary files for annotation seeking
		{
			// bool kara{lineInfo[8] == "kara"};

			tmp2 = "cue-" + to_string(i); // reset this

			if (line > 0) //
			{
				if (kara)
				{

					tmp2 = karaStr + "-" + tmp2 + "-" + to_string(k); // add the kara-prefix to subsequent kara lines
					// }
					k++;
				}
				else if (prevLine[8] == karaStr) // we won't get any errors on this bc we set the prevLine in the last line > 0 thing
				{
					k = 0; // reset k, but only if we just got off a karaoke line
				}
			}

			if (moreLines)
			{
				if (lines[line + 1][8] != karaStr)
				{
					i++; // increment only if the next line isn't a kara line
				}
				else if (lines[line + 1][8] == karaStr) // we can only be doing karaoke mode if there are y'know, future lines to karaoke anyway...
				{
					karaMode = true; // don't switch this off until later
				}
			}

			// aaaaaaaand print!
			cstr << tmp2 << endl; // give each cue their numbered id
			if (!kara)
			{
				// maybe later add  << (fx != "" ? " " + fx : "") to also add the labelled effects as a class
				hstr << "<div class=\"" << htmlClasses[0] << "\"><h4>" << tmp2 << "<br />"; // don't close the el yet
			}
		}

		(lyricLine ? ostr : cstr) << startCue << " --> " << endCue << endl; // timing. requires an extra 0 at the end, though preceeding 0's are fine and whatever
		tmp = lineInfo[9];
		(lyricLine ? ostr : cstr) << tmp << endl
								  << endl; // text

		// now again, only want to do this to the commentary, non-lyric lines
		if (!lyricLine)
		{

			if (!kara)
			{
				// so if we're not in a karaoke line, then we will have to be writing a timestamp somehow. it's just a matter of whether it's the start or the end time...

				hstr << "<span class=\"timestamp\">" << startCue << " --&gt; "; // well. if we're not on a karaoke line, then we'll always be needing the opening if we're not on a kara opening
			}

			if (karaMode) // i'm sure there's a more efficient way to do this with try / catch, but i don't feel like handling that rn
			{
				if (moreLines)
				{
					if (lines[line + 1][8] != karaStr)
					{
						// if the next one is not a karaoke line
						karaMode = false;
					}
				}
				else
				{
					karaMode = false; // switch this off if there aren't any more lines anyway
				}
			}

			if (!karaMode)
			{
				hstr << endCue << "</span></h4>" << endl; // go ahead and close this off once we're out of karaoke skipping mode

				// writeHTML();							  // just write it normally if we're not in karaoke mode. duh

				stmp.clear(); // now we clear this out again for reuse
				stmp << tmp;  // we feed it tmp
				//
				while (getline(stmp, tmp2))
				{
					hstr << "\t<p>" << tmp2 << "</p>" << endl; // and now for each line of tmp, we make a paragraph of commentary. should probably also be sanitizing the inputs here but it's fine for now
				}
				hstr << "</div>" << endl; // and um. don't forget to close the div at the end too lmao
			}
		}

		if (line == (lines.size() - 1) && !lyricLine && endCue < subTime(endTime))
		{
			// if we're on the last one, then add the empty gap line for the last empty bits too
			cstr << "gap-" << j << endl;
			cstr << endCue << " --> " << endTime << endl;
			cstr << " ";

			hstr << "<div class=\"" << htmlClasses[0] << "\"><h4>gap-" << j << "<br /><span class=\"" << htmlClasses[1] << "\">" << endCue << " --&gt; " << endTime << "</span></h4>\n<p>&nbsp;</p></div>" << endl; // write the gaps to the thing too
		}
	}

	hstr << "</div>\n</details>"; // close the scrolly div and the details element
	hstr << endl
		 << endl
		 << endl
		 << endl
		 << endl; // and then add a couple of newlines at the end to help identify anything that didn't get overwritten

	// if this goes well, then could def optimize the script by just using tuples n pairs, but for now, don't worry abt that

	cstr.close();
	cstr.clear();
	// quickOpen(cstr, string(directory.string() + "/"))
	quickOpen(cstr, string(directory.string() + (subpath != "" ? "/" : "") + subpath + "/comm/ao3/" + ass.name() + "-comm-ao3.vtt")); // make the commentary track
	cstr << "WEBVTT\n\n\n";

	karaMode = false;
	vector<subTime> timer;
	timer.assign(times.begin(), times.end());
	// what we should do. is we should alter freminet to be another cue vector, n he can hold the lyrics, while lynette holds the annotations, and then as we loop through the timer, we use i and j n stuff to basically track where we should start in the lyntwin vectors, and we first loop through the lyric vector to print the appearing lyrics, then we loop through the annotations vector to print the appearing annotations
	i = 0;
	j = 0;
	for (int ts{0}; ts < timer.size() - 1; ts++)
	{ // ts for timestamp lol
		startCue = timer[ts];
		endCue = timer[ts + 1];
		cstr << "cue-Lyr" << i << "-Anno" << j << endl;
		cstr << startCue << " --> " << endCue << endl;
		// lyney and the lyrics
		printLine(lyney, startCue, endCue, i);
		// lynette and the annotations
		printLine(lynette, startCue, endCue, j);
		cstr << endl;
	}

	// clear these vectors
	lyney.clear();
	lynette.clear();

	cout << "Complete." << endl;
	// close the streams lol
	istr.close();
	ostr.close();
	cstr.close();
	hstr.close();
}

void printLine(vector<cue> &v, subTime s, subTime e, int &q)
{
	// always uses cstr
	for (int i = q; i < v.size(); i++)
	{
		const cue l = v[i];

		if (l.startTime > e) // if the line we're looking at starts after this current one ends
		{
			break;
		}
		if (l.endTime <= s)
		{			// so if the line we're looking at ends before the current line even starts
			q++; // then we can start from there next time
		}
		
		// "A" in this case is the current line (c)'s timestamps; it is indexed by int l
		// "B" in this case is lynette[i]; it is indexed by int i
		// to overlap lines moving forward means that A.start <= B.start < A.end; this happens when i >= l
		// to overlap lines looking backward means that B.start < A.start && B.end > A.start; this happens when i < l
		// if B.start > A.end, we have passed the overlap section and should break
		if (btwnIU(s, make_pair(l.startTime, l.endTime)) || btwnUI(e, make_pair(l.startTime, l.endTime)))
		{
			cstr << l << endl;
		}
	}
}

//