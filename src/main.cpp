#include <iostream>
#include <iomanip>
#include "math.h"
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include <regex>
#include <filesystem>
#include "./subfiles/subtitle.h"

#ifndef PI
#define PI 3.14159265358979 // can't seem to find out how to get a constant from a library in here so this will be good enough
#endif						// ends def of pi

using namespace std;

string tmp{""}; // tmp for the getline
filesystem::path subfile{""}, directory{".."};
fstream istr, ostr; // io file streams
vector<filesystem::path> available;

// function loop for showing the available ass and srt files
void showFiles();
void quickOpen(fstream &stream, string path);
void quickOpen(fstream &stream, filesystem::path path);
void SRtoVT();
void ASStoVT();
string rgba(string); // takes a string as the ass file takes it and converts it to rgba
int htod(string c);

int main()
{
	// filesystem::directory_entry currDir{directory}; // turn the string into a directory lol
	cout << "Files available for conversion:" << endl;
	showFiles();
	int choice{0};
	cout << "Please choose a file to convert: ";
	cin >> choice;
	while (choice > available.size())
	{
		cout << "hey you can't do that. choose another number: ";
		cin >> choice;
	}
	if (cin.good())
	{
		subfile = available[choice - 1]; // set the subfile to the chosen one
	}

	subtitle subs(subfile); // makes a subtitle object with the chosen file
	cout << "Now working with " << subs.name() << "." << subs.ext() << endl;
	// cout << "("<< subs.path() <<")\n";
	quickOpen(istr, subs.path());								  // open the relevant subtitle file
	quickOpen(ostr, string("../output/" + subs.name() + ".vtt")); // make the output file
	ostr.clear();												  // clear the output stream before we start writing
	ostr << "WEBVTT\n\n\n";										  // write this at the top
	if (subs.ext() == "srt")
	{
		SRtoVT();
	}
	else if (subs.ext() == "ass")
	{
		ASStoVT();
	}

	cout << "Complete." << endl;
	// close the streams lol
	istr.close();
	ostr.close();
	return 0;
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

void SRtoVT()
{
	bool timingLine{false};		// boolean for whether the following line has the timing in it
	int currLine{1}, tmpInt{0}; // integer for the current line in the .srt file, as well as a temp for checking if we're currently
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
			cout << "Line " << currLine << ".\n";
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
}

void ASStoVT()
{
	while (getline(istr, tmp))
	{
		// if (regex_search(tmp, regex("\\[V4\\+?\\s\\Styles]", "i"))) {
		if (tmp == "[V4+ Styles]")
		{
			cout << "Styling about to start.\n";
			break;
		}
	}
	ostr << "STYLE" << endl;

	vector<string> lineInfo{};
	stringstream stmp{""};
	string tmp2;
	// while loop to process the styles
	while (getline(istr, tmp))
	{
		// the first line SHOULD be just the format
		if (tmp == "[Events]" || !regex_search(tmp, regex("(\\w|\\d)+"))) // the second one looking for Literally Any Word Characters is important or else we get some sort of uncaught error when doing the stringstream
		{
			cout << "End of the styles." << endl;
			break; // leave when the events start or there's a blank line
		}
		tmp = regex_replace(tmp, regex("\\w+:\\s+"), ""); // remove the first thing
		stmp << tmp; // write tmp to the sstream
		int i{0};
		while (getline(stmp, tmp2, ','))
		{ // comma-separated
			cout << i << ". " << tmp2 << "\t";
			lineInfo.push_back(tmp2);
			i++;
		}
		cout << endl;
		stmp.clear();

		float fontSize{0.0};
		try
		{
			fontSize = stof(lineInfo[2]); // first, we should test to make sure it's been properly formatted
			ostr << "::cue" << (lineInfo[0] != "Default" ? "(." + lineInfo[0] + ")" : "");
			ostr << "{ font-family: \"" << lineInfo[1] << "\";";
			ostr << " font-size: " << fontSize << "pt; ";
			ostr << "font-weight: " << ((stoi(lineInfo[7]) % 2 != 0) ? "bold" : "normal") << "; ";
			ostr << "font-style: " << ((stoi(lineInfo[8]) % 2 != 0) ? "italic" : "regular") << "; ";
			// ostr << "text-decoration: " << ";"; // yeah don't bother with the
			if (stoi(lineInfo[15]) == 1)
			{
				// uses a text outline; loop for thing
				ostr << "text-shadow: ";
				int width{stoi(lineInfo[16])};
				string outlineColor = rgba(lineInfo[5]);
				// cout << "width of " << width << endl;
				while (width > 0)
				{
					int steps{int(pow(2, width + 1))};
					// cout << steps << " steps around the circle.\n";
					for (int i{0}; i < steps; i++)
					{
						float angle = (2 * i * PI) / steps;
						ostr << width * round(100 * cos(angle)) / 100 << "px " << width * round(100 * sin(angle)) / 100 << "px " << (width - 1) / 2 << "px " << outlineColor; // << ","; // takes these in radians
						ostr << (((i != steps - 1 && width - 1 != 0)) ? "," : ";");																							  // add a comma if we're not literally the last one here; otherwise, semicolon
					}
					width--;
				}
			}
			else if (stoi(lineInfo[15]) == 3)
			{
				// this means it uses a background box rather than a text outline
				ostr << "background-color: " << rgba(lineInfo[6]) << "; ";
			}
			ostr << " color: " << rgba(lineInfo[3]) << "; ";
			ostr << "margin: " << lineInfo[21] << "px " << lineInfo[20] << "px " << lineInfo[19] << "px; ";
			ostr << "letter-spacing: " << 1 - (round(100 * stoi(lineInfo[11])) / 10000) << "em; ";
			ostr << "}" << endl; // prints the closing thing char(125)
		}
		catch (exception)
		{
			cout << "This .ass file is improperly formatted." << endl;
		}
	}

	lineInfo.clear();
	stmp.clear();
	int i{0}; // int to hold onto which sub-thing we're currently on
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
	ostr << "\n\n"; // add the extra lines
	getline(istr, tmp);								  // now we get the line After
	tmp = regex_replace(tmp, regex("\\w+:\\s+"), ""); // clear off the colon n stuff
	stmp << tmp;									  // write tmp to the string stream
	while (getline(stmp, tmp2, ','))
	{
		cout << i << ". " << tmp2 << "\t";
		lineInfo.push_back(tmp2);
		i++;
	}
	cout << endl;
	vector<vector<string>> lines{}; // a vector for all the lines, so that later it can be iterated through and examined for timing things
	const int formats = lineInfo.size();
	cout << "there are " << formats << " items in the vector." << endl;
	while (getline(istr, tmp))
	{
		// now we get to processing the actual meat of it
		if (!regex_search(tmp, regex("$Comment:\\s"))) // ignore comments for now, though .vtt supports them
		{
			lineInfo.clear();
			stmp.clear();
			i = 0;
			// if it's not a comment, then it must be a dialogue
			tmp = regex_replace(tmp, regex("$(\\w|\\d)+:\\s+"), ""); // clear off the "Dialogue: "

			stmp << tmp;

			while(getline(stmp, tmp2, ',')) {
				if (i < formats) {
					lineInfo.push_back(tmp2);
					i++;
				} else {
					lineInfo[formats - 1] += tmp2; //concatenate the dialogue lines even when they have commas :D
				}
			}

			lines.push_back(lineInfo); // add the line to the vector
		}
	}

	// now we loop through that 2d vector
	for (int line{0}; line < lines.size(); line++) {
		lineInfo = lines[line]; // i'm not setting aside more memory for a new vector. why would i do that.
		ostr << lineInfo[1] << " --> " << lineInfo[2] << endl; // timing
		ostr << lineInfo[9] << endl << endl; // text
	}
}

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