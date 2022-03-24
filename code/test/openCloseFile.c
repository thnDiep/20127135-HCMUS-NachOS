/*
Open and close the file 
input open(): the given name
input close(): OpenFileID

first, open the file by the given name
if open successfully --> close the file

*/

#include "syscall.h"

int main() {
    char* name;
    OpenFileId result;

    result = Open(name);
    if (result == 0) {
        Close(result);
    }
}