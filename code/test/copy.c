/* copy.c
 *	Simple program to test whether the systemcall interface works.
 *	
 *	Just do ReadString, PrintString, Create, Seek, Open, Read, Write, Close syscall that copies the content of the source file for destination file.
 *
 */

#include "syscall.h" 

int main () {
	int srcFile, desFile;	// id of files
	char nameSrcFile[32], nameDesFile[32];
	int lengthName, sizeFile, i;
	char info; 
	
	// Read the source file name
    PrintString("\nInput source file name's length: ");
    lengthName = ReadNum();
	PrintString("Input source file name: ");
	ReadString(nameSrcFile, lengthName);
	
	// Read the destination file name
    PrintString("\nInput destination file name's length: ");
    lengthName = ReadNum();
	PrintString("Input destination file name: ");
	ReadString(nameDesFile, lengthName); 

	// Open the source file
	srcFile = Open(nameSrcFile); 

	if (srcFile == -1) {
		PrintString ("Open source file fail.\n");
		Halt();
	}

    if (Create(nameDesFile) == -1){
		PrintString("Create destination file fail!.\n");
		Halt();
	}

	// Open the destination file
	desFile = Open(nameDesFile); 
        
	if (desFile == -1){
		PrintString ("Open destination file fail.\n");
		Halt();
	}

	// copy the content of the source file for destination file
	sizeFile = Seek(-1, srcFile); 	// Size of the source file

	Seek(0, srcFile); 				// Go to the top of the file to start reading file
	Seek(0, desFile); 				// Go to the top of the file to start writing file

	for (i = 0; i < sizeFile; i++) 
	{
		Read(&info, 1, srcFile);	// Read character from the source file
		Write(&info, 1, desFile); 	// Write it into the destination file
	}

	Close(desFile); 
	Close(srcFile); 

	Halt();
	/*  Not reached  */
}
