/* help.c
 *	Simple program to print ....... (dịch ra TA giùm i =)))
 *	in ra các dòng giới thiệu cơ bản về nhóm và mô tả vắn tắt về chương trình sort, ascii, createfile
 *	...
 *
 */

#include "syscall.h"

int main(){
    PrintString("\t\t -----GROUP-----\n");
	PrintString("20127135 - Tran Huynh Ngoc Diep\n");
	PrintString("20127252 - Ngo Gia Ngan\n");
	PrintString("20127305 - Tran Thao Quyen\n");
	
	PrintString("\t\t -----ASCII-----\n");
	PrintString("Program print the ASCII table\n\n");

	PrintString("\t\t -----Sort-----\n");
	PrintString("User can input an array of n integers (n <= 100)\n");
	PrintString("Using algorithm bubble sort to\nsort the array in ascending\n(descending), depending on user\n\n");
	// Thiếu createfile, cat, copy, delete
	Halt();
	  /* not reached */
}