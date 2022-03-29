/* delete.c 
*  Simple program to test whether the systemcall interface works.
*
*  Just do ReadString, PrintString and Remove syscall that delete the file
*        return 1 on success, -1 on failure
*
*/



#include "syscall.h"

int main() {
    char name[255];
    PrintString("Input the name of file\n");
    ReadString(name,readNum());

    Remove(name);

    Halt();
    /*  Not reached  */
}