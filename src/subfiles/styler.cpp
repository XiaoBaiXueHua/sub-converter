#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
// #include <regex>
#include <vector>
#include "math.h"
#include "options.h"

using namespace std;

/* color class */
color::color(string c)
{
	ass = c;
	a = htod(c.substr(2, 2));
	r = htod(c.substr(8, 2));
	g = htod(c.substr(6, 2));
	b = htod(c.substr(4, 2));
}

int color::htod(string c)
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

// operator overloading
ostream &operator<<(ostream &os, const color &c)
{
	os << "rgba(" << c.r << ", " << c.g << ", " << c.b << (1 - round(100 * c.a)) << ")";
	return os;
}

stylesheet::stylesheet(string line)
{
	// Format: Name (0), Fontname (1), Fontsize (2), PrimaryColour (3), SecondaryColour (4), OutlineColour (5), BackColour (6), Bold (7), Italic (8), Underline (9), StrikeOut (10), ScaleX (11), ScaleY (12), Spacing (13), Angle (14), BorderStyle (15), Outline (16), Shadow (17), Alignment (18), MarginL (19), MarginR (20), MarginV (21), Encoding (22)
	line = line.substr(7, line.length()); // cuts off the "Style: " bit at the start
	stringstream wah(line);				  // make a stringstream
	string tmp{""};
	vector<string> vals = {};
	while (getline(wah, tmp, ','))
	{
		// get the comma-separated values
		vals.push_back(tmp);
	}
	styleName = vals[0];
	fontName = vals[1];
	align = vals[18];
	fontSize = stof(vals[2]);
	fontSpacing = stof(vals[13]);
	border = stof(vals[16]);
	borderStyle = stoi(vals[15]);
	// marginL = stoi(vals[19]);
	// marginR = stoi(vals[20]);
	// marginV = stoi(vals[21]);
	// cout << "constructing the colors..." << endl;
	primary = color(vals[3]);
	secondary = color(vals[4]);
	outline = color(vals[5]);
	back = color(vals[6]);
	// cout << "constructing the bools..." << endl;
	bold = abs(stoi(vals[7]));
	italic = abs(stoi(vals[8]));
	underline = abs(stoi(vals[9]));
	strikeout = abs(stoi(vals[10]));
}

ostream &operator<<(ostream &os, const stylesheet &sheet)
{
	os << "::cue";
	if (sheet.styleName != "Default")
	{
		os << "(." + sheet.styleName + ")";
	}
	os << " { font-family: " << quoted(sheet.fontName) << "; ";
	os << "font-size: " << sheet.fontSize << "pt; ";
	os << "font-weight: " << (sheet.bold ? "bold" : "normal") << "; ";
	os << "font-style: " << (sheet.italic ? "italic" : "regular") << "; ";
	if (sheet.border > 0)
	{
		// so if there's a border
		// string ol{sheet.outline.rgba()}; // save the outline so as not to make five jillion function calls
		if (sheet.borderStyle == 1)
		{
			// if it's an outline, do elaborate shit
			if (options::styleBorders)
			{
				os << "text-shadow: ";
				int width{int(round(sheet.border))};
				while (width > 0)
				{
					int steps{int(pow(2, width + 1))};
					for (int i{0}; i < steps; i++)
					{
						float angle = (2 * i * PI) / steps;
						os << width * round(100 * cos(angle)) / 100 << "px " << width * round(100 * sin(angle)) / 100 << "px " << (width - 1) / 2 << "px " << sheet.outline; // << ","; // takes these in radians

						os << (((i != steps - 1 && width - 1 != 0)) ? "," : ";"); // add a comma if we're not literally the last one here; otherwise, semicolon
					}
					width--;
				}
			}
		}
		else if (sheet.borderStyle == 3)
		{
			// otherwise it's a background color
			os << "background-color: " << sheet.outline << "; ";
		}
	}
	os << "color: " << sheet.primary << "; ";
	// could theoretically add margins but eh
	if (sheet.fontSpacing > 0)
	{
		os << "letter-spacing: " << 1 - (round(100 * sheet.fontSpacing) / 10000) << "em; ";
	}
	os << "}";
	return os;
}

//