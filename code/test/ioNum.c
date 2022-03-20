/* ioNum.c
 *	Simple program to test whether the systemcall interface works.
 *	
 *	Just do a ReadNum and PrintNum syscall.
 *
 */

#include "syscall.h"

int main(){
  int result;
  
  result = ReadNum();
  PrintNum(result);

  Halt();
  /* not reached */
}