/* ascii.c
 *	Simple program to test whether the systemcall interface works.
 *	
 *	Just do PrintNum and PrintChar syscall that prints the ascii table.
 *
 */

#include "syscall.h"

int main() {
	int i;

	for(i = 32; i < 127; i++){
		PrintNum(i);
		PrintChar('\t');
		PrintChar((char)i);
		PrintChar('\n');
	}
	Halt();
	  /* not reached */
}
