/* ioChar.c
 *	Simple program to test whether the systemcall interface works.
 *	
 *	Just do a ReadChar and PrintChar syscall.
 *
 */

#include "syscall.h"

int main(){
  char result;
  
  result = ReadChar();
  PrintChar(result);

  Halt();
  /* not reached */
}