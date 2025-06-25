
// int main()
// {
// 	// filesystem::directory_entry currDir{directory}; // turn the string into a directory lol
// 	cout << "Files available for conversion:" << endl;
// 	showFiles();
// 	int choice{0};
// 	cout << "Please choose a file to convert: ";
// 	cin >> choice;
// 	while (choice > available.size())
// 	{
// 		cout << "hey you can't do that. choose another number: ";
// 		cin >> choice;
// 	}
// 	if (cin.good())
// 	{
// 		subfile = available[choice - 1]; // set the subfile to the chosen one
// 	}

// 	subtitle subs(subfile); // makes a subtitle object with the chosen file
// 	cout << "Now working with " << subs.name() << "." << subs.ext() << endl;
// 	// cout << "("<< subs.path() <<")\n";
// 	quickOpen(istr, subs.path());								  // open the relevant subtitle file
// 	quickOpen(ostr, string("../output/" + subs.name() + ".vtt")); // make the output file
// 	ostr.clear();												  // clear the output stream before we start writing
// 	ostr << "WEBVTT\n\n\n";										  // write this at the top
// 	if (subs.ext() == "srt")
// 	{
// 		SRtoVT();
// 	}
// 	else if (subs.ext() == "ass")
// 	{
// 		ASStoVTT();
// 	}

// 	cout << "Complete." << endl;
// 	// close the streams lol
// 	istr.close();
// 	ostr.close();
// 	return 0;
// }