/* cat.c
 *	Simple program to test whether the systemcall interface works.
 *	
 *	Just do ReadString, PrintString, Open and Seek syscall that display the content of the file.
 *
 */



#include "syscall.h" 

int main() {
    // char name[100];
    // PrintString("Input name's length: ");
    // int n = ReadNum();
    // PrintString("\nInput name: ");
    // ReadString(name, n); 

    // int k = Open(name);
    // char info;
    // if (k != -1) {
    //     int size = Seek(-1, k); // lay do dai noi dung
    //     Seek(0,k);              // quay lai dau file de doc noi dung
    //     for(int i = 0; i < size; i++) {
    //         Read(&info, 1, k);
    //         PrintString(info);
    //     }
    //     Close (k);
    // }
    // else 
    //     PrintString ("Error!!");
    
    // return 0;
    Halt();
    /*  Not reached  */
}
