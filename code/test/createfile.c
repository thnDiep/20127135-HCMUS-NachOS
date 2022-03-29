/* create.c
 *	Simple program to test whether the systemcall interface works.
 *	
 *	Just do a Create syscall that creates the new file and return 0 if success, -1 if fail
 *
 */

#include "syscall.h"

int main() {
    int n = Create ("TestFile.txt");
    if (n == 0) 
        PrintString("Create file success!");
    else if (n == -1)
        PrintString("Create file fail!");
    else 
        PrintString("Error!");
    //Halt();

    /* Nhap ten file tu ng dung
    char name[100];
    PrintString("Input name's length: ");
    int n = ReadNum();
    PrintString("\nInput name: ");
    ReadString(name, n); 
    int m = Create(name);
     if (m == 0) 
        PrintString("Create file success!");
    else if (m == -1)
        PrintString("Create file fail!");
    else 
        PrintString("Error!");*/

    Halt();
    /*  Not reached  */
}


