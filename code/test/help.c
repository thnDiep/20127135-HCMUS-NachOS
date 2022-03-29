/* help.c
 *	Simple program to print basic introduction to groups,
 	descripe about sort, ascii, createfile, cat, copy, delete program
 */

#include "syscall.h"

int main(){
    PrintString("\t\t -----GROUP-----\n");
	PrintString("20127135 - Tran Huynh Ngoc Diep\n");
	PrintString("20127252 - Ngo Gia Ngan\n");
	PrintString("20127305 - Tran Thao Quyen\n");
	
	PrintString("\t\t -----ASCII-----\n");
	PrintString("Program print the ASCII table\n\n");

	PrintString("\t\t -----Sort-----\n");
	PrintString("User can input an array of n integers (n <= 100)\n");
	PrintString("Using algorithm bubble sort to\nsort the array in ascending\n(descending), depending on user\n\n");

	PrintString("\t\t -----Create File-----\n");
	PrintString("Program executes the command to create a new file 'TestFile.txt'\n");
	PrintString("Announce the result of the program: success or failure or error\n\n")

	PrintString("\t\t -----Cat-----\n");
	PrintSrting("User input the name of file\n");
	PrintString("Program print content of that file\n\n");

	PrintString("\t\t -----Copy-----\n");
	PrintString("User input the name of source and destination file\n");
	PrintString("Program copy content of source file for destination file\n\n");

	PrintString("\t\t -----Delete-----\n");
	PrintString("User input the name of file\n");
	PrintString("Using remove system call to delete the file if it exists and close\n\n");

	Halt();
	  /* not reached */
}