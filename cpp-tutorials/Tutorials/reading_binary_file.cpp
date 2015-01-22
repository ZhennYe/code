// binary files
// write and read - input/output binary data sequentially
//		write ( memory_block, size );
//		read  ( memory_block, size );
//	where memory_block is of type char* and represents an array of bytes where the
//		read data elements are stored (or from where the written data is taken)
//	size - int, specifies number of characters to be read or written to/from mem block

// reading a complete binary file example
#include <iostream>
#include <fstream>
using namespace std;

ifstream::pos_type size; // used for buffer and file positioning (returned by file.tellg()
char* memblock; // array of bytes where data is stored

int main () {
	ifstream file ("example.bin",
ios::in | ios::binary | ios::ate); // opens with ios::ate = pointer positioned @ end of file
	if (file.is_open())			// this means tellg() will return the size of the file
	{
		size = file.tellg(); //'int size;' would work for files <2GB
		memblock = new char [size]; //dynamic allocation of memblock
		file.seekg (0, ios::beg); // 0 offset counter from beginning of stream to 
	// set 'get' pointer at beginning of file (@ file open, pointer was set @ end of file)
		file.read (memblock, size); // store contents of file in memblock
		file.close();
		cout << "the complete file content is in memory.\n";
		delete[] memblock;
	}
	else cout << "Unable to open file.\n";
	return 0;
}

