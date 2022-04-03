// exception.cc 
//	Entry point into the Nachos kernel from user programs.
//	There are two kinds of things that can cause control to
//	transfer back to here from user code:
//
//	syscall -- The user code explicitly requests to call a procedure
//	in the Nachos kernel.  Right now, the only function we support is
//	"Halt".
//
//	exceptions -- The user code does something that the CPU can't handle.
//	For instance, accessing memory that doesn't exist, arithmetic errors,
//	etc.  
//
//	Interrupts (which can also cause control to transfer from user
//	code into the Nachos kernel) are handled elsewhere.
//
// For now, this only handles the Halt() system call.
// Everything else core dumps.
//
// Copyright (c) 1992-1996 The Regents of the University of California.
// All rights reserved.  See copyright.h for copyright notice and limitation 
// of liability and disclaimer of warranty provisions.
#include "copyright.h"
#include "main.h"
#include "syscall.h"
#include "ksyscall.h"
#include "synchconsole.h"
#include "filesys.h"
//----------------------------------------------------------------------
// ExceptionHandler
// 	Entry point into the Nachos kernel.  Called when a user program
//	is executing, and either does a syscall, or generates an addressing
//	or arithmetic exception.
//
// 	For system calls, the following is the calling convention:
//
// 	system call code -- r2
//		arg1 -- r4
//		arg2 -- r5
//		arg3 -- r6
//		arg4 -- r7
//
//	The result of the system call, if any, must be put back into r2. 
//
// If you are handling a system call, don't forget to increment the pc
// before returning. (Or else you'll loop making the same system call forever!)
//
//	"which" is the kind of exception.  The list of possible exceptions 
//	is in machine.h.
//----------------------------------------------------------------------
	
const int MAX_INT = 2147483647;
const int MIN_INT = -2147483648;
const int MAX_LENGTH_STRING = 255;
const int MAX_FILE_LENGTH = 32;


// Modify return point 
void IncreasePC(){
	  /* set previous programm counter (debugging only)*/
	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	  
	  /* set next programm counter for brach execution */
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
}

/* 
Input: - User space address (int) 
 - Limit of buffer (int) 
Output:- Buffer (char*) 
Purpose: Copy buffer from User memory space to System memory space 
*/ 
char* UserToKernel(int virtAddr, int limit)
{ 
	// Create kernel space to receive data from user space
 	char* kernelBuf = NULL; // Kernel space
 	kernelBuf = new char[limit + 1];
	 
 	if (kernelBuf == NULL) 
 		return kernelBuf; 

 	memset(kernelBuf, 0, limit + 1); 

	// Move data
	int character;
 	for (int i = 0; i < limit; i++) 
 	{ 
 		kernel->machine->ReadMem(virtAddr + i, 1, &character); 
 		kernelBuf[i] = (char)character;
 		if (character == 0) 
 			break; 
 	} 
 	return kernelBuf; 
} 

/* 
Input: - User space address (int) 
 - Limit of buffer (int) 
 - Buffer (char[]) 
Output:- Number of bytes copied (int) 
Purpose: Copy buffer from System memory space to User memory space 
*/ 
int KernelToUser(int virtAddr, int len, char* buffer)
{
	// If data of kernel space invalid or empty
	if (len < 0) 
		return -1;
	if (len == 0)
		return len;

	// Move data
	int i = 0;
	int character = 0;
	do{ 
		character = (int)buffer[i]; 
		kernel->machine->WriteMem(virtAddr + i, 1, character); 
		i++; 
 	} while(i < len && character != 0);

	return i;
}

// ------------------------------------------------------------------------
// int Add(int op1, int op2);
// Use: Add the two operants and return the result
// ------------------------------------------------------------------------
void Handle_Add(){
	DEBUG(dbgSys, "Add " << kernel->machine->ReadRegister(4) << " + " << kernel->machine->ReadRegister(5) << "\n");
	
	/* Process SysAdd Systemcall*/
	int result;
	result = SysAdd(/* int op1 */(int)kernel->machine->ReadRegister(4),
					/* int op2 */(int)kernel->machine->ReadRegister(5));

	DEBUG(dbgSys, "Add returning with " << result << "\n");
	/* Prepare Result */
	kernel->machine->WriteRegister(2, (int)result);
}

// ------------------------------------------------------------------------
// int ReadNum();
// Use: Read the integer number input by user and return it or 0 (if not a integer)
// ------------------------------------------------------------------------
void Handle_ReadNum(){
	DEBUG(dbgSys, "---------- READING THE NUMBER CHARACTER FROM THE CONSOLE ----------\n");

	char input;						// Get input from console
	int64_t result = 0;
	int length = 0;					// Length of the number to check limit of number
	bool isError = false;
	bool isFirstChar = true;
	bool isNegative = false;

	while((input = kernel->synchConsoleIn->GetChar()) != '\n'){
		if(isFirstChar){
			isFirstChar = false;
			if(input == '-'){
				isNegative = true;
				continue;
			}
		}

		length++;

		// Check input
		if(input >= '0' && input <= '9')
			result = result * 10 + (input - '0');
		else
			isError = true;

	}

	if(isNegative)
		result *= -1;

	// Check the value of the number which was input
	if(length > 10)
		isError = true;
	else
		if(result < MIN_INT || result > MAX_INT)
			isError = true;

	if(isError){
		DEBUG(dbgSys, "Error (Invalid input): This is not a integer number.\n");
		result = 0;
	}

	DEBUG(dbgSys, "Read the number returning with " << result << ".\n");
	
	kernel->machine->WriteRegister(2, (int)result);
}

// ------------------------------------------------------------------------
// void PrintNum(int number);
// Use: Print the integer number to the console
// ------------------------------------------------------------------------
void Handle_PrintNum(){
	DEBUG(dbgSys, "---------- PRINTING THE NUMBER CHARACTER TO THE CONSOLE ----------\n");

	int number = kernel->machine->ReadRegister(4);

	// Case number is 0
	if(number == 0){
		kernel->synchConsoleOut->PutChar('0');
		return;
	}

	bool isNegative = false;		// check the sign of the number
	int length = 0;					// length of the number

	if(number < 0){					// negative number
		isNegative = true;
		number *= -1;
		length = 1;
	}

	int temp_number = number;		// compute length of the number
	while(temp_number){
		temp_number /= 10;
		length++;
	}

	char* result = new char [length + 1];
	result[length] = '\0';

	int pos = length - 1;
	while(number){
		result[pos] = number % 10 + '0';
		number /= 10;
		pos--;
	}

	if(isNegative)
		result[0] = '-';

	for(int i = 0; i < length; i++){
		kernel->synchConsoleOut->PutChar(result[i]);
	}
	delete[] result;
}

// ------------------------------------------------------------------------
// char ReadChar();
// Use: Read the character input by user and return it
// ------------------------------------------------------------------------
void Handle_ReadChar(){
	DEBUG(dbgSys, "---------- READING A CHARACTER TO THE CONSOLE ----------\n");

	char result = kernel->synchConsoleIn->GetChar();
	
	if(result == '\n')
		DEBUG(dbgSys, "Character is empty.\n")
	else
		DEBUG(dbgSys, "Read the charactor returning with " << result << ".\n");
	
	kernel->machine->WriteRegister(2, (int)result);
}

// ------------------------------------------------------------------------
// void PrintChar(char character);
// Use: Print a character to the console
// ------------------------------------------------------------------------
void Handle_PrintChar(){
	DEBUG(dbgSys, "---------- PRINTING A CHARACTER TO THE CONSOLE ----------\n");

	char result = (char)kernel->machine->ReadRegister(4);

	kernel->synchConsoleOut->PutChar(result);
}

// ------------------------------------------------------------------------
// int RandomNum();
// Use: Random and return the positive integer number
// ------------------------------------------------------------------------
void Handle_RandomNum(){
	DEBUG(dbgSys, "---------- RANDOM A POSITIVE INTEGER NUMBER ----------\n");

	srand(time(0));
	unsigned int result = rand();

	DEBUG(dbgSys, "Random returning with " << result << ".\n");
	kernel->machine->WriteRegister(2, result);
}

// ------------------------------------------------------------------------
// void ReadString(char buffer[], int length);
// Use: Read a string input by user from the console
// ------------------------------------------------------------------------
void Handle_ReadString() {
	DEBUG(dbgSys, "---------- READING A STRING FROM THE CONSOLE ----------\n");

	int addr = kernel->machine->ReadRegister(4);	// address of string
	int length = kernel->machine->ReadRegister(5);	// length of string
	
	char* buffer = new char[length + 1];

	char input;
	int pos = 0;
	while((input = kernel->synchConsoleIn->GetChar()) != '\n'){
		buffer[pos] = input;
		pos++;
		if(pos >= length)
			break;
	}
	buffer[pos] = '\0';

	KernelToUser(addr, length, buffer);
	delete[] buffer;
}

// ------------------------------------------------------------------------
// void PrintString(char buffer[]);
// Use: Print a string to the console
// ------------------------------------------------------------------------
void Handle_PrintString() {
	DEBUG(dbgSys, "---------- PRINTING A STRING TO THE CONSOLE ----------\n");

	int addr = kernel->machine->ReadRegister(4);	// address of string

	char* buffer = UserToKernel(addr, MAX_LENGTH_STRING);

	int i = 0;
	while (buffer[i] != 0 && buffer[i] != '\0'){
		kernel->synchConsoleOut->PutChar(buffer[i]);
		i++;
	}

	delete[] buffer;
}

// ------------------------------------------------------------------------
// int Create(char *name);
// Use: To create a Nachos file, with name "name"
// Return: 0 on success, -1 on failure
// ------------------------------------------------------------------------
void Handle_Create() {
	DEBUG(dbgSys, "---------- CREATING THE FILE ----------\n");

	int addr = kernel->machine->ReadRegister(4);  	// the address of name's file
	
	char* nameFile = UserToKernel(addr, MAX_FILE_LENGTH);

	// Check the name of files
	{
		if (strlen(nameFile) == 0){
			DEBUG( dbgSys, "Filename is invalid.\n");
			kernel->machine->WriteRegister(2, -1);
			delete[] nameFile;
			return;
		}
	
		if (nameFile == NULL){
			DEBUG(dbgSys, "Not enough memory in system.\n");
			kernel->machine->WriteRegister(2, -1);
			delete[] nameFile;
			return;
		}
	}
	
	// Craete file
	if (!kernel->fileSystem->Create(nameFile)){
		DEBUG(dbgSys, "Error when create file.\n");
		kernel->machine->WriteRegister(2, -1);
	}
	else{
		DEBUG(dbgSys, "Successfully create.\n");
		kernel->machine->WriteRegister(2, 0);
	}
	delete[] nameFile;	
}

// ------------------------------------------------------------------------
// int Remove(char *name);
// Use: To remove a Nachos file, with name "name"
// Return: 1 on success, -1 on failure
// ------------------------------------------------------------------------
void Handle_Remove() {
	DEBUG(dbgSys, "---------- REMOVING THE FILE ----------\n");
	int addr = kernel->machine->ReadRegister(4);	// the address of the name's file
	
	char* nameFile = UserToKernel(addr, MAX_FILE_LENGTH);
	if(kernel->fileSystem->Remove(nameFile)){
		DEBUG(dbgSys, "Successfully remove.\n");
		kernel->machine->WriteRegister(2, 1);
	}
	else{
		DEBUG(dbgSys, "The file doesn't exit.\n");
		kernel->machine->WriteRegister(2, -1);
	}
	delete[] nameFile;
}

// ------------------------------------------------------------------------
// OpenFileId Open(char *name);
// Use: To open a file with the user selected type of the file
// Return: OpenFileId on success, -1 on failure
// ------------------------------------------------------------------------
void Handle_Open() {
	DEBUG(dbgSys, "---------- OPENING THE FILE ----------\n");
	int addr = kernel->machine->ReadRegister(4);	// the address of the name's file
    
	char* nameFile = UserToKernel(addr, MAX_FILE_LENGTH);

	int freeSlot = kernel->fileSystem->FindFreeSlot();

	if (freeSlot != -1){  							// The opening file table has a free slot
		kernel->fileSystem->openingFile[freeSlot] = kernel->fileSystem->Open(nameFile);

		if (kernel->fileSystem->openingFile[freeSlot]) {
			DEBUG(dbgSys, "Open a file returning with " << freeSlot << ".\n");
			kernel->machine->WriteRegister(2, freeSlot);
		}
		else {
			DEBUG(dbgSys, "The file doesn't exit.\n");
			kernel->machine->WriteRegister(2, -1);
		}	
	}
	else{											// The opening file table is full
		DEBUG(dbgSys, "Can't open the file.\n");
		kernel->machine->WriteRegister(2, -1);
	}
	delete[] nameFile;
}

// ------------------------------------------------------------------------
// int Close(OpenFileId id);
// Use: To close the file
// Return: 1 on success, -1 on failure
// ------------------------------------------------------------------------
void Handle_Close() {
	DEBUG(dbgSys, "---------- CLOSING THE FILE ----------\n");
	OpenFileId id = kernel->machine->ReadRegister(4);

	if(id < 0 || id > kernel->fileSystem->maxOpeningFile - 1){
														// id out of range of the opening file table [0; 9]
		DEBUG(dbgSys, "ID of the file is out of range.\n");
		kernel->machine->WriteRegister(2, -1);
	}
	else{
		if (kernel->fileSystem->openingFile[id]){		// If the file is opening
			delete kernel->fileSystem->openingFile[id];
			kernel->fileSystem->openingFile[id] = NULL;

			DEBUG(dbgSys, "Successfully close.\n");
			kernel->machine->WriteRegister(2, 1);
		}
	}
}

// ------------------------------------------------------------------------
// int Read(char *buffer, int size, OpenFileId id);
// Use: To read "size" bytes from the open file into "buffer".
// Return: the number of bytes actually read, -1 on failure, -2 if read the empty file
// ------------------------------------------------------------------------
void Handle_Read() {
	DEBUG(dbgSys, "---------- READING THE FILE ----------\n");
	int addr = kernel->machine->ReadRegister(4);	// the address of the buffer
	int size = kernel->machine->ReadRegister(5);	// the size bytes of buffer
	OpenFileId id = kernel->machine->ReadRegister(6);

	// Possible errors
	{
		if(id < 0 || id > kernel->fileSystem->maxOpeningFile - 1){	
			DEBUG(dbgSys, "ID of the file is out of range.\n");
			kernel->machine->WriteRegister(2, -1);
			return;
		}

		if(kernel->fileSystem->openingFile[id] == NULL){
			DEBUG(dbgSys, "The file hasn't opened yet.\n");
			kernel->machine->WriteRegister(2, -1);
			return;
		}

		if(id == ConsoleOut){
			DEBUG(dbgSys, "Can't read the stdout file.\n");
			kernel->machine->WriteRegister(2, -1);
			return;
		}
	}
	

	int posBeforeRead = kernel->fileSystem->openingFile[id]->GetCurrentPos();
	char* buffer = new char[size];

	// Read the ConsoleInput
	if (id == ConsoleIn){
		// Read from console
		int number_byte = 0;
		char input;
		while((input = kernel->synchConsoleIn->GetChar()) != '\n'){
			buffer[number_byte] = input;
			number_byte++;
			if(number_byte >= size)
				break;
		}

		KernelToUser(addr, number_byte, buffer);

		DEBUG(dbgSys, "Read the file returning with " << number_byte << ".\n");
		kernel->machine->WriteRegister(2, number_byte);

		delete[] buffer;
		return;
	}

	// Read the file
	if ((kernel->fileSystem->openingFile[id]->Read(buffer, size)) > 0){
		int posAfterRead = kernel->fileSystem->openingFile[id]->GetCurrentPos();
		int number_byte = posAfterRead - posBeforeRead;

		KernelToUser(addr, number_byte, buffer); 

		DEBUG(dbgSys, "Read the file returning with " << number_byte << ".\n");
		kernel->machine->WriteRegister(2, number_byte);
	}
	else{
		DEBUG(dbgSys, "The file is empty.\n");
		kernel->machine->WriteRegister(2, -2);
	}

	delete[] buffer;
}

// ------------------------------------------------------------------------
// int Write(char *buffer, int size, OpenFileId id);
// Use: To write "size" bytes from "buffer" to the open file. 
// Return: the number of bytes actually write, -1 on failure
// ------------------------------------------------------------------------
void Handle_Write() {
	DEBUG(dbgSys, "---------- WRITING THE FILE ----------\n");
	int addr = kernel->machine->ReadRegister(4);	// the address of the buffer
	int size = kernel->machine->ReadRegister(5); 	// the size bytes of buffer
	OpenFileId id = kernel->machine->ReadRegister(6);

	// Possible errors
	{
		if(id < 0 || id > kernel->fileSystem->maxOpeningFile - 1){
			DEBUG(dbgSys, "ID of the file is out of range.\n");
			kernel->machine->WriteRegister(2, -1);
			return;
		}

		if(kernel->fileSystem->openingFile[id] == NULL){
			DEBUG(dbgSys, "The file hasn't opened yet.\n");
			kernel->machine->WriteRegister(2, -1);
			return;
		}

		if(id == ConsoleIn){
			DEBUG(dbgSys, "Can't write the stdin file.\n");
			kernel->machine->WriteRegister(2, -1);
			return;
		}
	}

	int posBeforeWrite = kernel->fileSystem->openingFile[id]->GetCurrentPos();
	char* buffer = UserToKernel(addr, size);
	
	// Write the ConsoleOutput
	if (id == ConsoleOut){
		int i = 0;
		char input;
		while (buffer[i] != 0 && buffer[i] != '\n'){
			kernel->synchConsoleOut->PutChar(buffer[i]);
			i++;
		}

		DEBUG(dbgSys, "Write the file returning with " << i - 1 << ".\n");
		kernel->machine->WriteRegister(2, i - 1);
		delete[] buffer;
		return;
	}

	// Write the file
	if ((kernel->fileSystem->openingFile[id]->Write(buffer, size)) > 0){
		int posAfterWrite = kernel->fileSystem->openingFile[id]->GetCurrentPos();
		int number_byte = posAfterWrite - posBeforeWrite;

		DEBUG(dbgSys, "Write the file returning with " << number_byte << ".\n");
		kernel->machine->WriteRegister(2, number_byte);
	}
	else{
		DEBUG(dbgSys, "Can't write the file.\n");
		kernel->machine->WriteRegister(2, -1);
	}

	delete[] buffer;
}

// ------------------------------------------------------------------------
// int Seek(int position, OpenFileId id);
// Use: To set the seek position of the open file "id" to the byte "position".
// Return: position on success, -1 on failure
// ------------------------------------------------------------------------
void Handle_Seek() {
	DEBUG(dbgSys, "---------- SEEKING THE FILE ----------\n");
	int pos = kernel->machine->ReadRegister(4);			// The position user want to seek
	OpenFileId id = kernel->machine->ReadRegister(5); 

	// Possible errors
	{
		if(id < 0 || id > kernel->fileSystem->maxOpeningFile - 1){
			DEBUG(dbgSys, "ID of the file is out of range.\n");
			kernel->machine->WriteRegister(2, -1);
			return;
		}

		if(kernel->fileSystem->openingFile[id] == NULL){
			DEBUG(dbgSys, "The file hasn't opened yet.\n");
			kernel->machine->WriteRegister(2, -1);
			return;
		}

		if(id == ConsoleIn || id == ConsoleOut){
			DEBUG(dbgSys, "Can't call System-call Seek on the console.\n");
			kernel->machine->WriteRegister(2, -1);
			return;
		}
	}
		
	int len = kernel->fileSystem->openingFile[id]->Length();
	
	// If pos = -1 go to the end of the file
	if(pos == -1)
		pos = len;

	if (pos < 0 || pos > len) {
		DEBUG(dbgSys, "Can't seek the file to this position.\n");
		kernel->machine->WriteRegister(2, -1);
	}
	else {
		kernel->fileSystem->openingFile[id]->Seek(pos);
		DEBUG(dbgSys, "Seek the file returning with " << pos << ".\n");
		kernel->machine->WriteRegister(2, pos);
	}
}

void ExceptionHandler(ExceptionType which){
    int type = kernel->machine->ReadRegister(2);

    DEBUG(dbgSys, "Received Exception " << which << " type: " << type << "\n");

    switch (which) {
		case NoException:
			return;

		case PageFaultException:
			cerr << "PageFaultException: No valid translation found.\n";
			SysHalt();
			break;

		case ReadOnlyException:
			cerr << "ReadOnlyException: Write attempted to page marked 'read-only'.\n";
			SysHalt();
			break;

		case BusErrorException:
			cerr << "BusErrorException: Translation resulted in an invalid physical address.\n";
			SysHalt();
			break;

		case AddressErrorException:
			cerr << "AddressErrorException: Unaligned reference or one that was beyond the end of the address space.\n";
			SysHalt();
			break;

		case OverflowException:
			cerr << "OverflowException: Integer overflow in add or sub.\n";
			SysHalt();
			break;

		case IllegalInstrException:
			cerr << "IllegalInstrException: Unimplemented or reserved instr.\n";
			SysHalt();
			break;

		case NumExceptionTypes:
			cerr << "NumExceptionTypes\n";
			SysHalt();
			break;

    	case SyscallException:
		{
			switch(type) {
      			case SC_Halt:
					DEBUG(dbgSys, "Shutdown, initiated by user program.\n");

					SysHalt();

					ASSERTNOTREACHED();
					break;

      			case SC_Add:
					Handle_Add();
					IncreasePC();
					return;
	
					ASSERTNOTREACHED();
					break;

				case SC_ReadNum:
					Handle_ReadNum();
					IncreasePC();
					return;
					
					ASSERTNOTREACHED();
					break;

				case SC_PrintNum:
					Handle_PrintNum();
					IncreasePC();
					return;
					
					ASSERTNOTREACHED();
					break;

				case SC_ReadChar:
					Handle_ReadChar();
					IncreasePC();
					return;

					ASSERTNOTREACHED();
					break;

				case SC_PrintChar:
					Handle_PrintChar();
					IncreasePC();
					return;

					ASSERTNOTREACHED();
					break;
				
				case SC_RandomNum:
					Handle_RandomNum();
					IncreasePC();
					return;

					ASSERTNOTREACHED();
					break;
				
				case SC_ReadString:
					Handle_ReadString();
					IncreasePC();
					return;

					ASSERTNOTREACHED();
					break;
				
				case SC_PrintString:
					Handle_PrintString();
					IncreasePC();
					return;

					ASSERTNOTREACHED();
					break;

				case SC_Create:
					Handle_Create();
					IncreasePC();
					return;

					ASSERTNOTREACHED();
					break;

				case SC_Remove:
					Handle_Remove();
					IncreasePC();
					return;

					ASSERTNOTREACHED();
					break;

				case SC_Open:
					Handle_Open();
					IncreasePC();
					return;

					ASSERTNOTREACHED();
					break;

				case SC_Close:
					Handle_Close();
					IncreasePC();
					return;

					ASSERTNOTREACHED();
					break;

				case SC_Read:
					Handle_Read();
					IncreasePC();
					return;

					ASSERTNOTREACHED();
					break;

				case SC_Write:
					Handle_Write();
					IncreasePC();
					return;

					ASSERTNOTREACHED();
					break;

				case SC_Seek:
					Handle_Seek();
					IncreasePC();
					return;

					ASSERTNOTREACHED();
					break;

     			default:
					cerr << "Unexpected system call " << type << "\n";
					break;
    		}
		}	
			break;

		default:
			cerr << "Unexpected user mode exception" << (int)which << "\n";
			break;
    }
    ASSERTNOTREACHED();
}
