//open (and close) a file
// used within following classes:
// ofstream - stream class to write files
// ifstream - stream to read from files
// fstream  - stream class to both read and write from/to files

// open (filename, mode);
//		filename - null-terminated char sequence of type const char*, same as string
//		literals. mode - optional with a combination of the following flags:
//	ios::in Open for input operations.
//	ios::out Open for output operations.
//	ios::binary Open in binary mode.
//	ios::ate Set the initial position at the end of the file. If this flag is not
//		 set to any value, the initial position is the beginning of the file.
//	ios::app All output operations are performed at the end of the file, appending the 
//		content to the current content of the file. This flag can only be used in streams
//		open for output-only operations.
//	ios::trunc If the file opened for output

// can be combined with bitwise operator | so that (no matter the class) the file can be
//		written to. i.e.:
// 		ofstream myfile;
//		myfile.open ("example.bin", ios::out | ios::app | ios::binary);
// For ifstream and ofstream classes, ios::in and ios::out are automatically assumed, even if a
// mode that does not include them is passed as second argument to the open() member function

// each open() member functions of classes has a default mode is used if file is opened
//		without second argument (no mode argument)
// class		default mode parameter
// ofstream 	ios::out
// ifstream 	ios::in
// fstream 		ios::in | ios::out (|=OR)

// check if files is open with is_open() - no arguments
//		if (myfile.is_open()) { //evaluate code }

// close a file: close()
//		myfile.close();
// a destrucuted file automatically calls this member function
