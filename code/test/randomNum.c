/* randomNum.c
 *	Simple program to test whether the systemcall interface works.
 *	
 *	Just do a RandomNum syscall and return the positive integer number.
 *
 */

#include "syscall.h"

int main(){
  int result;
  
  result = RandomNum();
  PrintNum(result);

  Halt();
  /* not reached */
}