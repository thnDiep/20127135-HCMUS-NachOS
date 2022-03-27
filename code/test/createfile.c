/* create.c
 *	Simple program to test whether the systemcall interface works.
 *	
 *	Just do a Create syscall that create the new file and return 0 if success, -1 if fail
 *
 */

#include "syscall.h"

int main() {
    // char name[255];
    int id;
    char buffer [255];
    // PrintString("Input the file's name: \n");
    // ReadString(name, 255);

    // if(Create(name) == 0)
    //     PrintString("Creating the new file successes!\n");
    // else
    //     PrintString("Creating the new file fails!\n");

    id = Open("file1", 1);
    if(Read(buffer, 10, id) >= 0)
        PrintString(buffer);
    
    Seek(5, id);
    Write(" all the ", 9, id);
    Close(id);
    // Remove("file1");
    Halt();

    /*  Not reached  */
}