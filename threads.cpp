#include <avr/io.h>
#include "threads.hpp"
#include "stack_magic.hpp"

#define RETURN_ADDRESS_OFFSET 1

namespace Threads {
	struct Settings settings;
	
	Thread *currentThread;
	
	// Public management functions
	void init(uint16_t stackSize) {
		settings.stackSize = stackSize;
		
		currentThread = new Thread;
		currentThread->pid = 0;
		currentThread->sreg = SREG;
		currentThread->address = 0;
		currentThread->stackptr = SP;
		currentThread->stackbase = RAMEND;
		currentThread->next = currentThread;
	}
	
	uint16_t createThread(void (*func)(void)) {
		// Convert address from little to big endian
		uint16_t newAddress = (uint16_t) func;
		uint8_t high = newAddress >> 8, low = (uint8_t) newAddress;
		newAddress = low << 8 | high;
		
		// Allocate resources
		uint8_t *newStack = new uint8_t[settings.stackSize];
		
		// Setup new thread
		Thread *newThread = new Thread;
		newThread->pid = getNextPID();
		newThread->sreg = SREG;
		newThread->address = newAddress;
		newThread->stackptr = (uint16_t) newStack + settings.stackSize;
		newThread->stackbase = (uint16_t) newStack;
		newThread->next = (Thread*) currentThread;
		getLastThread()->next = newThread;
		return newThread->pid;
	}

	// Private management functions
	uint16_t getNextPID() {
		uint16_t highestPID = 0;
		Thread *thread = currentThread;
		while (thread->pid > highestPID) {
			thread = thread->next;
		}
		return highestPID + 1;
	}
	
	Thread *getLastThread() {
		Thread *thread = currentThread;
		while (thread->next != currentThread) {
			thread = thread->next;
		}
		return thread;
	}		
	
	void switchThread() {
		uint16_t *ptr;

		ptr = (uint16_t*)(SP + RETURN_ADDRESS_OFFSET);
		
		// Save current state
		currentThread->sreg = SREG;
		currentThread->address = ptr[0];
		currentThread->stackptr = SP;
		
		// Switch to next thread
		currentThread = currentThread->next;
		
		// Restore state
		asm("cli");
		SP = currentThread->stackptr;
		// asm("sei");
		*((uint16_t*)(SP + RETURN_ADDRESS_OFFSET)) = currentThread->address;
		SREG = currentThread->sreg;		
	}
	
	void yield() {
		SM_PUSH_ALL_REGISTERS()
		Threads::switchThread();
		SM_POP_ALL_REGISTERS()
	}
}