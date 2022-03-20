/* readNum.c
 *	Simple program to test whether the systemcall interface works.
 *	
 *	Just do a readNum syscall that reads 1 number and returns this number.
 *
 */

#include "syscall.h"

int main(){
  int result;
  
  result = ReadNum();

  Halt();
  /* not reached */
}