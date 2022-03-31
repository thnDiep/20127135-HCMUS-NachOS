/* delete.c 
*  Simple program to test whether the systemcall interface works.
*
*  Just do ReadString, PrintString and Remove syscall that delete the file
*        return 1 on success, -1 on failure
*
*/

#include "syscall.h"

int main() {
    char name[32];
    int lengthName;

    PrintString("\nInput the length of filename: ");
    lengthName = ReadNum();
    PrintString("Input filename: ");
    ReadString(name, lengthName);

    Remove(name);

    Halt();
    /*  Not reached  */
}