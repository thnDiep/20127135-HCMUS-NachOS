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

/* Modify return point */
void IncreasePC(){
	  /* set previous programm counter (debugging only)*/
	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	  
	  /* set next programm counter for brach execution */
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
}

// Input: - User space address (int) 
// - Limit of buffer (int) 
// Output:- Buffer (char*) 
// Purpose: Copy buffer from User memory space to System memory space 
char* User2System(int virtAddr,int limit) 
{ 
 	int i;// index 
 	int oneChar; 
 	char* kernelBuf = NULL; 
 	kernelBuf = new char[limit +1];//need for terminal string 
 	if (kernelBuf == NULL) 
 		return kernelBuf; 
 	memset(kernelBuf,0,limit+1); 
 	//printf("\n Filename u2s:"); 
 	for (i = 0 ; i < limit ;i++) 
 	{ 
 		machine->ReadMem(virtAddr+i,1,&oneChar); 
 		kernelBuf[i] = (char)oneChar; 
 		//printf("%c",kernelBuf[i]); 
 		if (oneChar == 0) 
 			break; 
 	} 
 	return kernelBuf; 
} 


// Input: - User space address (int)
// 	- Limit of buffer (int)
// 	- Buffer (char[])
// Output:- Number of bytes copied (int)
// Purpose: Copy buffer from System memory space to User memory space
int System2User(int virtAddr,int len,char* buffer)
{
	if (len < 0) return -1;
	if (len == 0)return len;
	int i = 0;
	int oneChar = 0 ;
	do{
		oneChar = (int)buffer[i];
		machine->WriteMem(virtAddr+i,1,oneChar);
		i++;
	}while(i < len && oneChar != 0);
	return i;
}

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

void Handle_ReadNum(){
	DEBUG(dbgSys, "Read the integer number.\n");

	char input;					// Get input from console
	int64_t result = 0;
	int length = 0;				// Length of number
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

	DEBUG(dbgSys, "Read the number returning with " << result << "\n");
	/* Prepare Result */
	kernel->machine->WriteRegister(2, (int)result);
}

void Handle_PrintNum(){
	int number = kernel->machine->ReadRegister(4);
	DEBUG(dbgSys, "Print the number " << number << " to console.\n");

	if(number == 0){
		kernel->synchConsoleOut->PutChar('0');
		kernel->synchConsoleOut->PutChar('\n');
		return;
	}

	bool isNegative = false;
	int length = 0;

	if(number < 0){
		isNegative = true;
		number *= -1;
		length = 1;
	}

	int temp_number = number;
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
	kernel->synchConsoleOut->PutChar('\n');
	delete result;
}

void Handle_ReadChar(){
	DEBUG(dbgSys, "Read a character.\n");

	char result = kernel->synchConsoleIn->GetChar();
	if(result == '\n')
		DEBUG(dbgSys, "Character is empty.\n")
	else
		DEBUG(dbgSys, "Read the charactor returning with " << result << "\n");
	/* Prepare Result */
	kernel->machine->WriteRegister(2, result);
}

void Handle_PrintChar(){
	char result = kernel->machine->ReadRegister(4);
	DEBUG(dbgSys, "Print the character " << result << " to console.\n");
	kernel->synchConsoleOut->PutChar(result);
	kernel->synchConsoleOut->PutChar('\n');
}

void Handle_RandomNum(){
	DEBUG(dbgSys, "Random a positive integer number.\n");

	srand(time(0));
	unsigned int result = rand();

	DEBUG(dbgSys, "Random returning with " << result << "\n");
	/* Prepare Result */
	kernel->machine->WriteRegister(2, result);
}


void Handle_ReadString() {
	DEBUG(dbgSys, "Read a string.\n");

	int addr = kernel->machine->ReadRegister(4);     // address of string from register4
	int length = kernel->machine->ReadRegister(5); //length of string from register5
	char* buffer = User2System(addr, length);

	int s = kernel->synchConsoleIn->Read(buffer, length);
	System2User(addr, s, buffer);
	delete[] buffer;
}



void Handle_PrintString() {

	DEBUG(dbgSys, "Print a string.\n");
	int addr = kernel->machine->ReadRegister(4);
	int length = 0;
	char* buffer = User2System(addr, 255);
	while (buffer[length] != '\0' && buffer[length] != '\n' && buffer[length] != 0 ) {
		length++;
	}
	buffer[length]='\0';
	kernel->synchConsoleOut->Write(buffer, length);
	delete[] buffer;
}

void Handle_Create() {		 //name: address of memory in user space 
	DEBUG(dbgSys, "Create a file.\n");
	int addr = kernel->machine->ReadRegister(4);  	//read addr of name's file
	int length = 32
	char* name = User2System(addr, length);
	if (strlen(name) == 0) {  //empty file
		DEBUG( dbgSys, "File is empty.\n");
		kernel->machine->WriteRegister(2,-1);
	}
	else if (name == NULL) {
		DEBUG(dbgSys, "Can't read the file\n");
		kernel->machine->WriteRegister(2,-1);
	}
	
	// reading file
	if (!kernel->fileSystem->Create(name,0)) {  //error create file
		DEBUG(dbgSys, "Error when create file\n");
		kernel->machine->WriteRegister(2,-1);
	}
	else {
		kernel->machine->WriteRegister(2,0);
	}
	delete[] name;
}


void Handle_Open() {
	int addr = kernel->machine->ReadRegister(4);   //give the address of the name
	char* name = User2System(addr, 32);

	if ( !kernel->fileSystem->Open(name)) {
		DEBUG(dbgSys, "Can't open the file\n");
		kernel->machine->WriteRegister(2,-1);
	}
	else {
		DEBUG(dbgSys, "The file was opened\n");
		kernel->machine->WriteRegister(2, 0);
	}
}


void Handle_Close() {
	OpenFileId id;
	id = kernel->machine->ReadRegister(4);   //give the address of the id
	
	if (kernel->fileSystem->Open(id) == NULL) {
		kernel->machine->WriteRegister(2,-1);
	}
	else {
		delete kernel->fileSystem->Open(id);
		kernel->fileSystem->Open(id)=NULL;
		kernel->machine->WriteRegister(2,0);
	}
}


void Handle_Read(char* buffer, int size, OpenFileId id) {

}

void Handle_Seek() {
	int pos = kernel->machine->ReadRegister(4); 
	int fileID = kernel->machine->ReadRegister(5); 

	if (fileID < 0 || fileID > 14 || kernel->fileSystem->openf[fileID] == NULL) {
		DEBUG(dbgSys, "\nID is out of search range or ID is doesn't exist.");
		kernel->machine->WriteRegister(2, -1);
		return;
	}
	//Seek tren console
	if (fileID == 0 || fileID == 1)	{
		DEBUG(dbgSys, "\nError!!Can't call Seek on the console.");
		kernel->machine->WriteRegister(2, -1);
		return;
	}
	int Len = kernel->fileSystem->openf[fileID]->Length();
	pos = (pos == -1) ? Len : pos; 

	if (pos > Len || pos < 0) {
		kernel->machine->WriteRegister(2, -1);
	}
	else {
		kernel->fileSystem->openf[fileID]->Seek(pos);
		kernel->machine->WriteRegister(2, pos);
	}
}

void Handle_Remove() {

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
					char[] buffer;
					int length = 0;
					Handle_ReadString(buffer, length);
					IncreasePC();
					return;

					ASSERTNOTREACHED();
					break;
				
				case SC_PrintString():
					char[] buffer;
					Handle_PrintString(buffer);
					IncreasePC();
					return;

					ASSERTNOTREACHED();
					break;

				case SC_Create():
					char* name;
					Handle_Create(name);
					IncreasePC();
					return;

					ASSERTNOTREACHED();
					break;

				case SC_Open():
					char* name;
					Handle_Open(name);
					IncreasePC();
					return;

					ASSERTNOTREACHED();
					break;
				
				case SC_Close():
					OpenFileId id;
					Handle_Close(id);
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
