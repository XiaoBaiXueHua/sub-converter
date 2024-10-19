// simple read-only class for the subtitle file types
#include <string>
#include <filesystem>

using namespace std;
class subtitle
{
public:
	subtitle() {}; // default constructor
	subtitle(filesystem::path file)
	{
		n = file.stem().string();
		e = file.extension().string().substr(1, file.extension().string().length()); // cuts off the . at the start bc i think that's a bother
		p = file;
	}

	string name() { return n; }
	string ext() { return e; }
	filesystem::path path() { return p; }
private:
	string e{""}, n{""}; // the name, extension, and relative path
	filesystem::path p{""}; // the relative path
};