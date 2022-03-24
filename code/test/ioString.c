#include "syscall.h"

int main(){
  char[] result;
  int length = 0;
  
  ReadString(result, length);
  PrintString(result);

  Halt();
  /* not reached */
}