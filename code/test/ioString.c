/* ioString.c
 *	Simple program to test whether the systemcall interface works.
 *	
 *	Just do ReadString and PrintString syscall
 *
 */

#include "syscall.h"

int main(){
  char str[255];
  
  ReadString(str, ReadNum());
  PrintString(str);

  Halt();
  /* not reached */
}