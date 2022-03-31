/* cat.c
 *	Simple program to test whether the systemcall interface works.
 *	
 *	Just do ReadString, PrintString, PrintChar, Open, Close, Read and Seek syscall that display the content of the file.
 *
 */

#include "syscall.h" 

int main() {
    char name[32];
    int lengthName, sizeFile, idFile, i;
    char info;

    PrintString("\nInput the length of filename: ");
    lengthName = ReadNum();
    PrintString("Input filename: ");
    ReadString(name, lengthName);

    idFile = Open(name);    // Open file

    if(idFile == -1){
        PrintString ("Open file fail.\n");
        Halt();
    }
  
    // Successfully open
    sizeFile = Seek(-1, idFile);    // Get the length of the file
    Seek(0, idFile);                // Go to the top of the file to start read file

    for(i = 0; i < sizeFile; i++){
       Read(&info, 1, idFile);
       PrintChar(info);
    }

    Close (idFile);

    Halt();
    /*  Not reached  */
}
