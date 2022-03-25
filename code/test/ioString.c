#include "syscall.h"

int main(){
  char str[255];
  int length = 0;
  
  ReadString(str, ReadNum());
  PrintString(str);

  Halt();
  /* not reached */
}