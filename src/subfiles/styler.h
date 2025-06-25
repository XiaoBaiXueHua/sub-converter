#ifndef ASS_STYLER
#define ASS_STYLER
#include <iostream>
#include <iomanip>
#include <string>
#include <sstream>
// #include <regex>
#include <vector>
#include "math.h"
#include "options.h"
#include "styler.h"

#ifndef PI
#define PI 3.14159265358979 // can't seem to find out how to get a constant from a library in here so this will be good enough
#endif						// ends def of pi

using namespace std;

class color
{
	// class for colors
public:
	color() {}
	color(string c);

	// this is for parsing the ass hex colors and turning them into rgba
	int htod(string c);
	friend ostream &operator<<(ostream &, const color &);

protected:
	string ass{""}; // this is the default constructor format where it uses the hex codes from ass files
	int r{0}, g{0}, b{0};
	float a{0.0};
};


class stylesheet
{
public:
	stylesheet() {} // default constructor
	stylesheet(string line);
	friend ostream &operator<<(ostream &, const stylesheet &); // now it's printing out the line
protected:
	string styleName{""}, fontName{""}, align{"center"};
	int borderStyle{1}; // borderStyle is either 1 or 3 depending on if you want a regular outline (1) or a box (3)
	// int marginL{0}, marginR{0}, marginV{0};
	float fontSize{0.0}, fontSpacing{0.0}, border{0.0};
	color primary, secondary, outline, back;
	bool bold{false}, italic{false}, underline{false}, strikeout{false};
};
#endif