#include <Arduino.h>
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
		currentThread->stackptr = SP;
		currentThread->stackbase = (uint8_t*) RAMEND;
		currentThread->next = currentThread;
	}
	
	uint16_t createThread(void (*func)(void)) {
		// Allocate required resources
		uint8_t *newStack = new uint8_t[settings.stackSize];
		Thread *newThread = new Thread;
		
		// Prepare thread
		newThread->pid = getNextPID();
		newThread->stackbase = newStack;
		
		// Get adjusted stack pointer and write entry address
		uint8_t* stackptr = initStack(newStack,func);
		newThread->stackptr = (uint16_t) stackptr;
		
		// Insert the new thread into the queue
		getLastThread()->next = newThread;
		newThread->next = currentThread;
		
		// Return PID for management purposes
		return newThread->pid;
	}
		
	void yield() {
		SM_SAVE_CONTEXT()

		// Save stack of current thread
		currentThread->stackptr = SP;
		
		// Switch threads
		currentThread = currentThread->next;
		
		// Restore stack of currentThread
		// As this is a critical 16 bit value, we cannot let interrupts occur
		asm("cli");
		SP = currentThread->stackptr;
		
		SM_RESTORE_CONTEXT()
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
	
	uint8_t *initStack(uint8_t* stackbase, void (*entry)(void)) {
		// We need to jump to the top of the stack and need a pointer
		uint8_t *ptr = (uint8_t*) (stackbase + settings.stackSize - 1);
		
		// Write exit return address
		uint16_t exitAddress = (uint16_t) Threads::exit;
		ptr[0] = (uint8_t) exitAddress;
		ptr[-1] = (uint8_t) (exitAddress >> 8);
		ptr -= 2;
		
		// Write entry address
		uint16_t entryAddress = (uint16_t) entry;
		ptr[0] = (uint8_t) entryAddress;
		ptr[-1] = (uint8_t) (entryAddress >> 8);
		ptr -= 2;
		
		// Simulate 32 pushed registers
		for (int i = 0; i < 32; i++) {
			ptr[-i] = 0;
		}
		ptr -= 32;
		
		// Write pushed SREG
		ptr[0] = SREG;
		ptr--;
		
		// This should be it
		return ptr;
	}
		
	void exit(void) {
		while (1) {
			Threads::yield();
		}
	}
	
	void switchContext(void) {
		currentThread->stackptr = SP;
		currentThread = currentThread->next;
		asm("cli");
		SP = currentThread->stackptr;
	}
	
}