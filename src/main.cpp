#include <iostream>
#include <iomanip>
#include "math.h"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <regex>
#include <filesystem>
#include "subfiles/friends.h"
#include "subfiles/friends.cpp"
#include "subfiles/subtitle.h"
#include "subfiles/subtitle.cpp"
#include "subfiles/sub-friends.h"
#include "subfiles/sub-friends.cpp"
#include "subfiles/styler.h"
#include "subfiles/styler.cpp"

using namespace std;

string tmp{""}, tmp2{""}; // my beloved, versatile tmps for everything
stringstream stmp{""};

// default paths n values n whatnot for the configuration
// should probably make these all static variables eventually
filesystem::path subfile{""}, directory{".."};
// string options::output{"subspath"};

fstream istr, ostr, cstr, hstr; // io file streams, w/cstr being for the commentary file & hstr being the html file for ao3 later
smatch smidge;					// for when we're matching things

vector<filesystem::path> available;
vector<vector<string>> lines{}, display{}; // a vector for all the lines, so that later it can be iterated through and examined for timing things
vector<string> lineInfo{};				   // a vector for strings

pair<subTime, subTime> timestamps;
cue freminet; // so. it's the pair of timestamps, the style, the effects, and the line string
vector<cue> lyney, lynette, lyntwins;
vector<stylesheet> styles{};

int i{1}, j{0}, k{0}; // our beloved three random job integers
int opt{0};
bool miscSwitch{false};

// function loop for showing the available ass and srt files
void showFiles();
void showFiles(string ext);
int csvInput(fstream &stream, string fname);

void SRTtoVTT(subtitle);
// void ASStoVTT();
void ASStoVTT(subtitle);
stringstream getLine(vector<cue> &, subTime, subTime, int &);
void printLine(fstream &, vector<cue> &, subTime, subTime, int &);

int main()
{
	config(); // read the config file
	if (options::split)
	{
		configFanmix(); // configure this
	}
	// ellipses are not working rn so we'll do this the long way i suppose
	k = csvInput(ostr, options::csv.string()); // k gets used for karaoke too, yeah, but it doesn't get used again in the main function, so we can transform it as we please in the other files
	// int hi = 1;

	if (options::csvMode)
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
					subfile = locator(toLower(soup.title()), soup.ext());
					soup.setPath(subfile);
					(toLower(soup.ext()) == "ass") ? ASStoVTT(soup) : SRTtoVTT(soup);
					cout << endl;
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
				subfile = locator(tmp, soup.ext());
				soup.setPath(subfile);
				(toLower(soup.ext()) == "ass") ? ASStoVTT(soup) : SRTtoVTT(soup);
				cout << endl;
			}
			catch (exception &e)
			{
				cout << "whoops! couldn't find that file. :(" << endl;
				cout << "(exception: " << e.what() << ")";
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
			for (auto &f : available) // this is a vector of filepaths
			{
				subtitle soup(f);
				((toLower(soup.ext()) == "ass") ? ASStoVTT(soup) : SRTtoVTT(soup)); // then loop through everything
				cout << endl;
			}
		}
		else
		{
			subtitle soup(available[k - 1]);
			((toLower(soup.ext()) == "ass") ? ASStoVTT(soup) : SRTtoVTT(soup)); // otherwise, only convert the one chosen
			cout << endl;
		}
	}

	cout << "Press any key to close. ";
	cin >> tmp;

	return 0;
}

void showFiles()
{
	int i{1};
	available.clear(); // clears the vector of available files
	for (auto const &dir_entry : filesystem::recursive_directory_iterator{filesystem::directory_entry{directory}})
	{
		if (!dir_entry.is_directory())
		{
			filesystem::path currPath{dir_entry.path()}; // turn it into a path object
			// cout << "File type " << currPath.extension() << endl;
			if (currPath.extension() == ".ass" || currPath.extension() == ".srt")
			{
				cout << setw(15) << i << ".\t" << currPath << endl;
				available.push_back(currPath);
				i++;
			}
		}
	}
}

void showFiles(string ext) // default are .ass files
{
	showFiles(available, ext); // version declared in friends.cpp
}

int csvInput(fstream &stream, string fname)
{
	options::csvMode = false;
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
				options::csvMode = true; // only do this if there are actually csv files available
										 // j = csvInput(stream, available[opt - 1].string()); // uhhh then in order to make this recursive we set k to smth
			}
			else
			{
				cout << "damn. you hae NO csv files. pick a file instead: ";
				showFiles(options::filetype);
				cout << "> ";
				cin >> opt;
			}
			break;
		}
		case 2:
		{
			cout << "\nFiles available for conversion:" << endl;
			showFiles(options::filetype);
			break;
		}
		default:
		{
			cout << "hi. you can't do that. gonna show you all the existing ass files instead: " << endl;
			// choice = 2; // set this for y'know, The Thing later
			cout << "\nFiles available for conversion:" << endl;
			showFiles(options::filetype);
		}
		}
	}
	else
	{
		// otherwise yeah it does exist so get to processing
		options::csvMode = true;
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
					lineInfo.push_back(string((j < 10) ? string("0" + to_string(j)) : to_string(j))); // adds in the track number for the thing
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
	if (options::csvMode)
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

void SRTtoVTT(subtitle srt)
{
	options::filetype = "srt";
	bool timingLine{false};		// boolean for whether the following line has the timing in it
	int currLine{1}, tmpInt{0}; // integer for the current line in the .srt file, as well as a temp for checking if we're currently

	quickOpen(istr, srt.path());
	quickOpen(ostr, string(outStrings(options::split ? fanmixOpts::lyrDir.string() : "", srt) + ".vtt"));
	ostr.clear();
	ostr << "WEBVTT\n\n\n"; // write this at the top
	while (getline(istr, tmp))
	{
		try
		{
			tmpInt = stoi(tmp);
			// cout << tmp;
		}
		catch (exception)
		{
			if (int(char(tmp[0])) < 0)
			{
				// don't know how this happens, but it seems to be the issue with ep 60's subs so.
				tmpInt = currLine; // for now, we'll just assume that we're supposed to be starting a new line
			}
			else
			{
				// this is commented out for now, but it's used to debug what's wrong with the line.
				// if (regex_search(tmp, regex("\\d+"))) {
				// 	cout << "int(" << int(char(tmp[0])) << "): " << char(tmp[0]) << endl << tmp << endl;
				// }
			}
		}
		if (tmpInt == currLine)
		{
			// cout << "Line " << currLine << ".\n";
			timingLine = true; // if this current number is the same as what line we're supposed to be on, set this to true
			currLine++;		   // and iterate this for the next loop
		}
		else if (timingLine)
		{
			ostr << regex_replace(tmp, regex(","), ".") << endl; // now, if it's a timing line, we replace the commas with periods and spit the line out into the new vtt file
			timingLine = false;									 // and then set this to false so that on the next loop we get
		}
		else
		{
			ostr << tmp << endl; // and then write the line to the file
		}
	}
	// ah. forgot to close them.
	istr.close();
	ostr.close();
}

// in the future, perhaps increase the number of allowed file splitting... maybe have a setting where the number is automatically determined by the number of styles present in the .ass file
// we could have like a vector of fstreams? or something to that effect; maybe have a parent class which contains the filestream + stylesheet, so that we can override operators n stuff
// that way, we could also have other settings, like whether we give the cues ids... perhaps they can also have their own html streams, in case those also want to be made separately?
// that said, the way multiple files are currently handled is pretty delicate n requires the .ass files be formatted in a particular way for the timers to be correct... perhaps do smth more akin to how the combined files are done? though they ignore karaoke stuff a bit at this time o(--(
void ASStoVTT(subtitle ass)
{
	options::filetype = "ass";
	vector<string> htmlClasses{"cue", "timestamp"}; // the html classes vector has the cue divs and timestamp spans
	lineInfo.clear();								// clear this out of whatever it had before
	styles.clear();

	cout << "Now working with " << ass.name() << "." << ass.ext() << endl;
	quickOpen(istr, ass.path()); // open the relevant subtitle file

	quickOpen(ostr, string(outStrings(options::split ? fanmixOpts::lyrDir.string() : "", ass) + ".vtt"));
	ostr.clear(); // make the output file
	ostr << "WEBVTT\n\n\n"; // write this at the top

	if (options::split) // the only use for splitter mode at this point is for the fanmix stuff so yee
	{
		// quickOpen(cstr, string((options::output.string() != "" ? "/" : "") + options::output.string() + "/" + fanmixOpts::commDir.string() + "/" + ass.name() + "-comm.vtt")); // make the commentary track

		quickOpen(cstr, string(outStrings(fanmixOpts::commDir.string(), ass, "comm") + ".vtt")); // hardcode for comm n html n stuff For Now

		cstr.clear(); // clear the output streams before we start writing
		cstr << "WEBVTT\n\n\n";

		if (fanmixOpts::makeHTML)
		{
			// quickOpen(hstr, string((options::output.string() != "" ? "/" : "") + options::output.string() + "/" + fanmixOpts::htmlDir.string() + "/" + (options::csvMode ? ass.track() + "-" : "") + ass.name() + "-comm.html")); // make the html file
			quickOpen(hstr, string(outStrings(fanmixOpts::htmlDir.string(), ass, "comm", (options::csvMode ? ass.track() : "")) + ".html"));

			hstr.clear();
			hstr << "<details class=\"annotations-controller\" open><summary>&nbsp;</summary></details>" << endl; // make the annotations controller

			hstr << "<video width=\"100%\" controls=\"controls\" controlslist=\"nodownload\" playsinline=\"playsinline\" crossorigin=\"anonymous\" preload=\"metadata\">";
			if (options::csvMode)
			{
				// if we're in csv mode, then we can also write the fallback youtube video
				hstr << "\n\thi. it appears that your browser does not support playing video in html5. wild, considering this day and age. <br />however, you can still listen to this song <a href=" << quoted(ass.url()) << ">on youtube</a>!";
			}
			hstr << "\n\t<source src=\"" << fanmixOpts::mp3_host_url << (fanmixOpts::mp3_host_ext != "" ? "/" : "") << fanmixOpts::mp3_host_ext << "/"; // now we start writing the source
			if (options::csvMode)
			{
				// if we're in csv mode, then that means we were granted Important Info which we can then use
				hstr << ass.track() << "-" << subtitle::pathify(ass.artist()) << "-" << subtitle::pathify(ass.title());
			}
			else
			{
				hstr << "[HI. DON'T FORGET TO FILL THIS OUT WITH NUMBER-ARTIST-TITLE]";
			}
			hstr << ".mp3\" type=\"audio/mp3\">";

			hstr << "\n\t<track srclang=\"en\" label=\"Annotations\" kind=\"subtitles\" src=\"" << fanmixOpts::sub_host_url << (fanmixOpts::sub_host_ext != "" ? "/" : "") << fanmixOpts::sub_host_ext << "/" + fanmixOpts::commDir.string() + "/" << ass.name() << "-comm.vtt\" default=\"default\">\n</video>" << endl; // open up the html file with this and write the boilerplate for the html"

			hstr << "<hr />" << endl; // and you know what. we deserve a horizontal line.

			hstr << "<details>\n\t<summary><h3 class=\"file-declaration\">WEBVTT</h3></summary>" << endl; // details n summary
			hstr << "<p></p>" << endl;																	  // extra gap
			hstr << "<div class=\"scrolly\">" << endl;													  // div for scrolling through the cues
		}
	}

	if (options::style)
	{
		// so if we're supposed to print out the style
		// fast forward to the styles
		while (getline(istr, tmp))
		{
			if (tmp == "[V4+ Styles]")
			{
				cout << "Begin Styling." << endl;
				break;
			}
		}
		getline(istr, tmp); // get that extra line of formatting
		while (getline(istr, tmp))
		{
			if (tmp == "[Events]" || !regex_search(tmp, regex("(\\w|\\d)+"))) // the second one looking for Literally Any Word Characters is important or else we get some sort of uncaught error when doing the stringstream
			{
				cout << "End of styles." << endl;
				ostr << endl
					 << endl
					 << endl; // add in the extra gap lines
				break;		  // leave when the events start or there's a blank line
			}
			stylesheet whee(tmp);
			// ostr << whee << endl;
			styles.push_back(whee);
		}
	}

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
	lyntwins.clear();
	lines.clear();											// clear this out of whatever it was used for last
	const int formats = 10;									// ASS specification will have this always be 10
	subTime startTime{"0:00:00.00"}, endTime{"0:00:00.00"}; // get these from the cue-gap comments

	miscSwitch = true; // thinking abt it, could probably reuse this elsewhere, since it only gets used that one time
	bool lyricLine{false}, moreLines{false}, kara{false};
	// cue::lyrics = false; // reset this
	string fx{""};
	set<subTime> times{};

	// turns all the ass file lines into a vector of strings
	while (getline(istr, tmp))
	{
		// now we get to processing the actual meat of it
		lineInfo.clear();
		stmp.clear();

		// the lineInfo processing can be replaced with just a constructor in the future, since we can construct these from raw dialogue lines now
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
		lyricLine = (lineInfo[3] == fanmixOpts::lyricStr), kara = (fx == fanmixOpts::karaStr);

		if (lineInfo[8] == "cue-gap") // if the effect is "cue-gap", then save that info
		{
			(miscSwitch ? startTime : endTime) = lineInfo[(miscSwitch ? 1 : 2)];
			miscSwitch = false; // switch this off lol
		}
		else if (!regex_search(tmp, regex("^Comment:\\s+"))) // ignore comments for now, though .vtt supports them
		{
			freminet = cue(subTime(lineInfo[1]), subTime(lineInfo[2]), lineInfo[3], lineInfo[8], lineInfo[9]);
			freminet.sanitize(); // clean up the dialogue, as we're now just feeding it raw
			lineInfo[9] = freminet.dial(); // temp fix before i change <lines> to hold cues instead of strings

			times.insert(freminet.startTime);
			times.insert(freminet.endTime);
			(lineInfo[3] == fanmixOpts::lyricStr) ? lyney.push_back(freminet) : lynette.push_back(freminet); // lyney has lyrics; lynette has annotations

			// as <lines> is just a vector of strings
			lyntwins.push_back(freminet); // add the line to the vector
		}
	}

	bool karaMode{false}; // a boolean to track if we're in karaoke mode
	subTime startCue, endCue;
	// now for the fanmix stuff, we make these other files by going through the lines vector
	cue prevLine, nextLine; // ugh but. i guess it seems i need another vector for the previous line too!!!
	cout << "lyntwins.size(): " << lyntwins.size() << endl;
	if (options::split)
	{
		// now we loop through that 2d vector
		i = 1; // set this to 1 as we use it to form ids for each cue
		j = 0; // j tracks the gaps
		k = 0; // k tracks the karaoke

		// cue::lyrics = true; // and now also turn this back on

		for (int line{0}; line < lyntwins.size(); line++)
		{
			// cout << "line " << line << endl;
			tmp = "";				// clear this out
			freminet = lyntwins[line]; // i'm not setting aside more memory for a new vector. why would i do that.
			cout << "printing line: " << line << ". ";
			fx = freminet.effect();
			// string fx{lineInfo[8]};		 // we'll always be working with these, so we can just define them i guess
			startCue = freminet.startTime, endCue = freminet.endTime;
			// um. don't forget to fill out the times set lol
			lyricLine = (freminet.s() == fanmixOpts::lyricStr), moreLines = (line < lyntwins.size() - 1), kara = (fx == fanmixOpts::karaStr);

			if (line > 0) // only do this when i > 1 is to make sure we're not forming some sort of gap btwn the last lyric line and the first commentary line lol
			{

				prevLine = lyntwins[line - 1];	   // if we can do this...
				string prevStyle{prevLine.s()}; // then we should also be able to get this info as well
				subTime prevEnd{prevLine.endTime};
				// bool ;

				if (prevEnd != startCue || (prevStyle == fanmixOpts::lyricStr && startCue >= 0)) // if we're working with non-continuous subs OR we go from the lyrics to the annotations
				{
					// if ((prevStyle == "Lyrics" && !lyricLine && startCue >= 0)) // if we're not on a lyric line, but the previous line WAS a lyric line, AND if the start cue is greater than 0, then that means we're doing the end/start gaps for the different files
					if ((prevStyle == fanmixOpts::lyricStr && !lyricLine && startCue >= 0)) // if we're not on a lyric line, but the previous line WAS a lyric line, AND if the start cue is greater than 0, then that means we're doing the end/start gaps for the different files
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

							if (fanmixOpts::makeHTML)
							{
								hstr << "<div class=\"" << htmlClasses[0] << "\"><h4>gap-" << j << "<br /><span class=\"" << htmlClasses[1] << "\">" << prevEnd << " --&gt; " << startCue << "</span></h4>\n<p>&nbsp;</p></div>" << endl; // write the gaps to the thing too // actually. i just realized we don't actually need the gaps in here.
							}

							j++; // also only increment j when actually Writing the gap identifiers lol
						}
					}
				}
			}
			if (moreLines) {
				// if there are more cues
				nextLine = lyntwins[line + 1];
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

						tmp2 = fanmixOpts::karaStr + "-" + tmp2 + "-" + to_string(k); // add the kara-prefix to subsequent kara lines
						// }
						k++;
					}
					else if (prevLine.effect() == fanmixOpts::karaStr) // we won't get any errors on this bc we set the prevLine in the last line > 0 thing
					{
						k = 0; // reset k, but only if we just got off a karaoke line
					}
				}

				if (moreLines)
				{
					if (nextLine.effect() != fanmixOpts::karaStr)
					{
						i++; // increment only if the next line isn't a kara line
					}
					else if (nextLine.effect() == fanmixOpts::karaStr) // we can only be doing karaoke mode if there are y'know, future lines to karaoke anyway...
					{
						karaMode = true; // don't switch this off until later
					}
				}

				// aaaaaaaand print!
				cstr << tmp2 << endl; // give each cue their numbered id
				if (!kara && fanmixOpts::makeHTML)
				{
					// maybe later add  << (fx != "" ? " " + fx : "") to also add the labelled effects as a class
					hstr << "<div class=\"" << htmlClasses[0] << "\"><h4>" << tmp2 << "<br />"; // don't close the el yet
				}
			}

			(lyricLine ? ostr : cstr) << startCue << " --> " << endCue << endl; // timing. requires an extra 0 at the end, though preceeding 0's are fine and whatever
			// tmp = lineInfo[9];
			(lyricLine ? ostr : cstr) << freminet << endl
									  << endl; // text

			// now again, only want to do this to the commentary, non-lyric lines
			if (!lyricLine)
			{

				if (!kara && fanmixOpts::makeHTML)
				{
					// so if we're not in a karaoke line, then we will have to be writing a timestamp somehow. it's just a matter of whether it's the start or the end time...

					hstr << "<span class=\"timestamp\">" << startCue << " --&gt; "; // well. if we're not on a karaoke line, then we'll always be needing the opening if we're not on a kara opening
				}

				if (karaMode) // i'm sure there's a more efficient way to do this with try / catch, but i don't feel like handling that rn
				{
					if (moreLines)
					{
						if (nextLine.effect() != fanmixOpts::karaStr)
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

				if (!karaMode && fanmixOpts::makeHTML)
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

			if (line == (lyntwins.size() - 1) && !lyricLine && endCue < subTime(endTime))
			{
				// if we're on the last one, then add the empty gap line for the last empty bits too
				cstr << "gap-" << j << endl;
				cstr << endCue << " --> " << endTime << endl;
				cstr << " ";

				if (fanmixOpts::makeHTML)
				{
					hstr << "<div class=\"" << htmlClasses[0] << "\"><h4>gap-" << j << "<br /><span class=\"" << htmlClasses[1] << "\">" << endCue << " --&gt; " << endTime << "</span></h4>\n<p>&nbsp;</p></div>" << endl; // write the gaps to the thing too
				}
			}
		}

		if (fanmixOpts::makeHTML)
		{
			hstr << "</div>\n</details>"; // close the scrolly div and the details element
			hstr << endl
				 << endl
				 << endl
				 << endl
				 << endl; // and then add a couple of newlines at the end to help identify anything that didn't get overwritten
		}
		// if this goes well, then could def optimize the script by just using tuples n pairs, but for now, don't worry abt that

		cstr.close();
		cstr.clear();
	}

	// we should theoretically be done with all the streams now
	istr.close();
	ostr.close();
	cstr.close();
	hstr.close();

	// now if we're Not Splitting and/or we want a combined file
	if (!options::split || fanmixOpts::combine)
	{
		// then we do this combination file
		quickOpen(ostr, string(outStrings((fanmixOpts::combine ? fanmixOpts::comboDir.string() : ""), ass, (fanmixOpts::combine ? "comm-ao3" : "")) + ".vtt")); // where it goes depends on if "combine" has been turned on

		ostr << "WEBVTT\n\n\n";

		karaMode = false;

		if (options::style)
		{
			ostr << "STYLE" << endl;
			// loop through the stylesheet
			for (auto const &s : styles)
			{
				ostr << s << endl;
			}
			ostr << endl
				 << endl
				 << endl;
		}

		vector<subTime> timer;
		timer.assign(times.begin(), times.end());
		// what we should do. is we should alter freminet to be another cue vector, n he can hold the lyrics, while lynette holds the annotations, and then as we loop through the timer, we use i and j n stuff to basically track where we should start in the lyntwin vectors, and we first loop through the lyric vector to print the appearing lyrics, then we loop through the annotations vector to print the appearing annotations
		i = 0;
		j = 0;
		for (int ts{0}; ts < timer.size() - 1; ts++)
		{ // ts for timestamp lol
			startCue = timer[ts];
			endCue = timer[ts + 1];
			// lyney and the lyrics
			if (options::gaps)
			{
				if (!fanmixOpts::combine) // at some point later, separate this out blahblahblah
				{
					ostr << "cue-Lyr" << i << "-Anno" << j << endl; // make an option to change these cue things at some point
				}
				// doing it like this will print the gaps unconditionally
				ostr << startCue << " --> " << endCue << endl;
				printLine(ostr, lyney, startCue, endCue, i);
				// lynette and the annotations
				printLine(ostr, lynette, startCue, endCue, j);
				ostr << endl;
			}
			else // otherwise we have to test them. will have to optimize this at some point such that it only returns cues
			{
				stringstream nyo, nah;
				nyo = getLine(lyney, startCue, endCue, i);
				nah = getLine(lynette, startCue, endCue, j);
				if ((!nyo.str().empty() || !nah.str().empty()))
				{
					if (!fanmixOpts::combine)
					{
						ostr << "cue-Lyr" << i << "-Anno" << j << endl; // make an option to change these cue things at some point
					}
					// so if at least one of them has a string
					ostr << startCue << " --> " << endCue << endl;
					printLine(ostr, lyney, startCue, endCue, i);
					// lynette and the annotations
					printLine(ostr, lynette, startCue, endCue, j);
					ostr << endl;
				}
			}
		}

		// clear these vectors
		lyney.clear();
		lynette.clear();
		lyntwins.clear();
		// close the outstream
		ostr.close();
	}
	fanmixOpts::combine = options::combine; // reset this
	// quickOpen(cstr, string(outStrings(fanmixOpts::comboDir.string(), ass, "comm-ao3") + ".vtt"));

	cout << "Complete." << endl;
}

// function for printing multiple overlapping lines as discrete combined cues
stringstream getLine(vector<cue> &v, subTime s, subTime e, int &q)
{
	stringstream hoo;
	for (int i = q; i < v.size(); i++)
	{
		const cue l = v[i];

		if (l.startTime > e) // if the line we're looking at starts after this current one ends
		{
			break;
		}

		// "A" in this case is the current line (c)'s timestamps; it is indexed by int l
		// "B" in this case is lynette[i]; it is indexed by int i
		// to overlap lines moving forward means that A.start <= B.start < A.end; this happens when i >= l
		// to overlap lines looking backward means that B.start < A.start && B.end > A.start; this happens when i < l
		// if B.start > A.end, we have passed the overlap section and should break
		if (btwnIU(s, make_pair(l.startTime, l.endTime)) || btwnUI(e, make_pair(l.startTime, l.endTime)))
		{
			if (l != "")
			{
				hoo << l << endl;
			}
		}
	}
	return hoo;
}
void printLine(fstream &stream, vector<cue> &v, subTime s, subTime e, int &q)
{
	for (int i = q; i < v.size(); i++)
	{
		const cue l = v[i];

		if (l.startTime > e) // if the line we're looking at starts after this current one ends
		{
			break;
		}
		if (l.endTime <= s)
		{		 // so if the line we're looking at ends before the current line even starts
			q++; // then we can start from there next time
		}

		// "A" in this case is the current line (c)'s timestamps; it is indexed by int l
		// "B" in this case is lynette[i]; it is indexed by int i
		// to overlap lines moving forward means that A.start <= B.start < A.end; this happens when i >= l
		// to overlap lines looking backward means that B.start < A.start && B.end > A.start; this happens when i < l
		// if B.start > A.end, we have passed the overlap section and should break
		if (btwnIU(s, make_pair(l.startTime, l.endTime)) || btwnUI(e, make_pair(l.startTime, l.endTime)))
		{
			stream << l << endl;
		}
	}
}

//