/* add.c
 *	Simple program to test whether the systemcall interface works.
 *	
 *	Just do PrintString, PrintString, Create, Seek, Close syscall that copies the content of the source file for destination file.
 *
 */

#include "syscall.h" 

int main () {
	// int file1, file2, sizeFile;
	// char name1[100], name2[100];
	
    // PrintString("Input file1 name's length: ");
    // int n = ReadNum();
	// PrintString("\nInput file1 name: ");
	// ReadString(name1, n); 
	
    // PrintString("Input file2 name's length: ");
    // int m = ReadNum();
	// PrintString("Input file2 name:");
	// ReadString(name2, m); 

	// file1 = Open(name1, 1); 
	// char info; 
	// if (file1 != -1) 
	// {
	// 	file2 = Create(name2);
	// 	Close(file2);
	// 	file2 = Open(name2, 0); 
	// 	if (file2 != -1) 
	// 	{
	// 		sizeFile = Seek(-1, file1);
	// 		Seek(0, file1); // Seek den dau file nguon
	// 		Seek(0, file2); // Seek den dau file dich
	// 		for (int i = 0 ; i < sizeFile; i++) 
	// 		{
	// 			Read(&info, 1, file1); //Doc tung ki tu cua file nguon
	// 			Write(&info, 1, file2); //Ghi vao file dich
	// 		}
	// 		PrintString("Successful!\n");
	// 		Close(file2); 
	// 	}
	// 	else
	// 		PrintString("Error\n");
		
	// 	Close(file1); 
	// }
	// else
	// 	PrintString("Can't open file!");
	
	// return 0;
	Halt();
	/*  Not reached  */
}
