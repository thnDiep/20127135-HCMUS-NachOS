/*
Create empty file
input: name
output: 0: success      1: fail 

*/


#include "syscall.h"


int main() {
    char* name;

    int result = create(name);
    //

    Halt();
    /*  Not reached  */
}