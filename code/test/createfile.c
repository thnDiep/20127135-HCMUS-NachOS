/* create.c
 *	Simple program to test whether the systemcall interface works.
 *	
 *	Just do a ReadString, PrintString, ReadNum, Create syscall that creates the new file and return 0 if success, -1 if fail
 *
 */

#include "syscall.h"

int main() {
    char name[] = "TestFile.txt";
    int type;                   // choice of user
    int length;                 // length of the filename input by user

    // User choose default or input
    PrintString("\n\t-----Menu-----\n");
    PrintString("1) Default filename.\n");
    PrintString("2) Input filename.\n");
    PrintString("Enter your choice (1 or 2): ");

    type = ReadNum();
    while (type != 1 && type != 2) {
        PrintString("Error, please try again: ");
        type = ReadNum();
    }

    // User input the filename
    if(type == 2){
        PrintString("\nInput the length of filename: ");
        length = ReadNum();
        PrintString("Input filename: ");
        ReadString(name, length); 
    }

    // Create file
    if (Create(name) == 0) 
        PrintString("Create file success!");
    else
        PrintString("Create file fail!");

    Halt();
    /*  Not reached  */
}


