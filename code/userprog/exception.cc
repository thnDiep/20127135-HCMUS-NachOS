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

// Modify return point 
void IncreasePC(){
	  /* set previous programm counter (debugging only)*/
	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	  
	  /* set next programm counter for brach execution */
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
}

// Moving data from user space to kernel space
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
 		// if (character == 0) 
 		// 	break; 
 	} 
 	return kernelBuf; 
} 

// Moving data from kernel space to user space
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
	// Trong chuỗi có quyền có ký tự 0 nên không dùng character == 0 thì ngắt được
	while(i < len){
		character = (int)buffer[i];
		kernel->machine->WriteMem(virtAddr + i, 1, character);
		i++;
	}

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
	DEBUG(dbgSys, "Read the character.\n");

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

// Mới chỉnh sửa read/print được bình thường chưa xét các trường hợp ngoại lệ 
void Handle_ReadString() {
	DEBUG(dbgSys, "Read the string.\n");

	int addr = kernel->machine->ReadRegister(4);	// address of string from register4
	int length = kernel->machine->ReadRegister(5);	// length of string from register5
	char* buffer = new char[length + 1];

	char input;
	int pos = 0;
	while((input = kernel->synchConsoleIn->GetChar()) != '\n'){
		buffer[pos] = input;
		pos++;
		if(pos >= length)
			break;
	}

	KernelToUser(addr, length, buffer);
	delete[] buffer;
}

void Handle_PrintString() {
	DEBUG(dbgSys, "Print the string.\n");

	int addr = kernel->machine->ReadRegister(4);

	char* buffer = UserToKernel(addr, MAX_LENGTH_STRING);

	// Count the length of the string
	int length = 0;
	while (buffer[length] != '\0')
		length++;

	// Print the string
	for(int i = 0; i< length; i++)
		kernel->synchConsoleOut->PutChar(buffer[i]);

	delete[] buffer;
}

void Handle_Create() {
	DEBUG(dbgSys, "Create a new file.\n");

	int addr = kernel->machine->ReadRegister(4);  	// read the address of name's file
	
	char* nameFile = UserToKernel(addr, MAX_LENGTH_STRING);

	// Check the name of files
	if (strlen(nameFile) == 0) {
		DEBUG( dbgSys, "Filename is invalid.\n");
		kernel->machine->WriteRegister(2, -1);
		delete nameFile;
		return;
	}
	
	if (nameFile == NULL) {
		DEBUG(dbgSys, "Can't read the file.\n");
		kernel->machine->WriteRegister(2, -1);
		delete nameFile;
		return;
	}

	// Craete file
	if (!kernel->fileSystem->Create(nameFile)) {	// error create file
		DEBUG(dbgSys, "Error when create file.\n");
		kernel->machine->WriteRegister(2, -1);
	}
	else 
		kernel->machine->WriteRegister(2, 0);
	delete nameFile;
}

void Handle_Open() {
	int addr = kernel->machine->ReadRegister(4);	// read the address of name's file
	int type = kernel->machine->ReadRegister(5);
    
	char* nameFile = UserToKernel(addr, MAX_LENGTH_STRING);

	int free = kernel->fileSystem->FindFreeSlot();
	if (free != -1) {  // khi slot trong
		if (type == 0 || type == 1){
			if ((kernel->fileSystem->openf[free] = kernel->fileSystem->Open(nameFile, type)) != NULL) {
				kernel->machine->WriteRegister(2, free);
			}
		}
		else if (type == 2 ) {
			kernel->machine->WriteRegister(2,0);
		}
		else {
			kernel->machine->WriteRegister(2,1);
		}
	}
	kernel->machine->WriteRegister(2,-1);

	delete nameFile;
}

void Handle_Close() {
	int fid =kernel->machine->ReadRegister(4); // Lay id cua file tu thanh ghi so 4
	if (fid >= 0 && fid <= 14) // Chi xu li khi fid nam trong [0, 14]
	{
		if (kernel->fileSystem->openf[fid]) //neu mo file thanh cong
		{
			delete kernel->fileSystem->openf[fid]; // Xoa vung nho luu tru file
			kernel->fileSystem->openf[fid] = NULL; // Gan vung nho NULL
			kernel->machine->WriteRegister(2, 0);
			return;
		}
	}
	kernel->machine->WriteRegister(2, -1);
}


// void Handle_Read(char* buffer, int size, OpenFileId id) {

// }

/*void Handle_Seek() {
	char *tmp;
	int pos = kernel->machine->ReadRegister(4); 
	OpenFileId fileID = kernel->machine->ReadRegister(5); 

	*tmp = (char) fileID;
	if (fileID < 0 || fileID > 14 || kernel->fileSystem->Open(tmp) == NULL) {
		DEBUG(dbgSys, "\nID is out of search range or ID is doesn't exist.");
		kernel->machine->WriteRegister(2, -1);
		return;
	}*/
	// //Seek tren console
	// if (fileID == 0 || fileID == 1)	{
	// 	DEBUG(dbgSys, "\nError!!Can't call Seek on the console.");
	// 	kernel->machine->WriteRegister(2, -1);
	// 	return;
	// }
	// int Len = kernel->fileSystem->Open(tmp)->Length();
	// pos = (pos == -1) ? Len : pos; 

	// if (pos > Len || pos < 0) {
	// 	kernel->machine->WriteRegister(2, -1);
	// }
	// else {
	// 	OpenFile* temp = kernel->fileSystem->Open(tmp);
	// 	temp->Seek(pos);
	// 	kernel->machine->WriteRegister(2, pos);
	// }
//}

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

				// case SC_Seek:
				// 	Handle_Seek();
				// 	ASSERTNOTREACHED();
				// 	break;

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
