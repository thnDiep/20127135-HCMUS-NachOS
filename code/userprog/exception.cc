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
	


/* Modify return point */
void IncreasePC(){
	  /* set previous programm counter (debugging only)*/
	  kernel->machine->WriteRegister(PrevPCReg, kernel->machine->ReadRegister(PCReg));

	  /* set programm counter to next instruction (all Instructions are 4 byte wide)*/
	  kernel->machine->WriteRegister(PCReg, kernel->machine->ReadRegister(PCReg) + 4);
	  
	  /* set next programm counter for brach execution */
	  kernel->machine->WriteRegister(NextPCReg, kernel->machine->ReadRegister(PCReg) + 4);
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
	DEBUG(dbgSys, "Read integer number \n");
	int result = 0;
	int length = 0;
	const int MAX_LENGHT = 9;
	char* buffer = new char [MAX_LENGHT + 1];
	length = kernel->synchConsoleIn->Read(buffer, MAX_LENGHT);
	bool isError = false;

	for(int i = 0; i < length; i++){
		if(buffer[i] >= '0' && buffer[i] <= '9'){
			result = result * 10 + (int)buffer[i];
		}
		else{
			isError = true;
			break;
		}
	}

	if(isError){
		DEBUG(dbgSys, "Error (Invalid input): This is not a integer number.\n");
		result = 0;
	}

	delete buffer;
	kernel->machine->WriteRegister(2, (int)result);
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
