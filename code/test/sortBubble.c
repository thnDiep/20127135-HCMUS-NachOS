/* sortBubble.c
 *	Simple program to test whether the systemcall interface works.
 *	
 *	Using bubble sort to sort a array ascending or descending and display the result
 *
 */
#include "syscall.h"
 
int main() {
    int array[100];
    int n, type, tmp;
    int i, j; //index in loop for

    // Input n
    PrintString("Enter the number of elements (0 < n <= 100): ");
    n = ReadNum();
    while (n <= 0 || n > 100) {
        PrintString("\nError, please try again: ");
        n = ReadNum();
    }


    // Input an integer array
    PrintString("\nInput array:\n");
    for (i = 0; i < n; i++) {
        PrintString("array[");
        PrintNum(i);
        PrintString("] = ");
        array[i] = ReadNum();
    }


    // User choose type to sort
    PrintString("\n\t-----Menu-----\n");
    PrintString("1) Ascending\n");
    PrintString("2) Descending\n");
    PrintString("Enter your choice (1 or 2): ");
    
    type = ReadNum();
    while (type != 1 && type != 2) {
        PrintString("Error, please try again: ");
        type = ReadNum();
    }
    
    
    // Output the integer array
    PrintString("\nYour array before sort:\n");
    for (i = 0; i < n; i++) {
        PrintNum(array[i]);
        PrintChar('\t');
    }

    
    // Sort array
    if (type == 1) {
        for (i = 0; i < n - 1; i++) {
            for (j = 0; j < n - i - 1; j++) {
                if (array[j] > array[j + 1]) {
                    tmp = array[j];
                    array[j] = array[j + 1];
                    array[j + 1] = tmp;
                }
            }
        }
    }
    else if (type == 2) {
        for (i = 0; i < n - 1; i++) {
            for (j = 0; j < n - i - 1; j++) {
                if (array[j] < array[j + 1]) {
                    tmp = array[j];
                    array[j] = array[j + 1];
                    array[j + 1] = tmp;
                }
            }
        }
    }
    
    // Output the integer array
    PrintString("\nYour array after sorting:\n");
    for (i = 0; i < n; i++) {
        PrintNum(array[i]);
        PrintChar('\t');
    }

    Halt();
     /* not reached */
}