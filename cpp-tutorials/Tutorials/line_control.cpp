// line control

// compiler gives reference to line # and name of file when an error is encountered
// #line directive allows control of both things
//		#line number "filename"
// number = the line number assigned to the next line of code
// "filename" = redefine the file name that will be shown

// example of a forced error after #line
//	 	#line 20 "assigning variable"
// 		int a?;
// will generate an error in file that the compiler will call "assigning variable" at line 20.
