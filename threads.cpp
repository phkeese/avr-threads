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
		currentThread->sreg = SREG;
		currentThread->stackptr = SP;
		currentThread->stackbase = RAMEND;
		currentThread->next = currentThread;
	}
	
	uint16_t createThread(void (*func)(void)) {
		// Allocate required resources
		uint8_t *newStack = new uint8_t[settings.stackSize];
		Thread *newThread;
		
		// Prepare thread
		newThread->pid = getNextPID();
		newThread->stackbase = (uint16_t) newStack;
		newThread->stackptr = (uint16_t) (newStack + settings.stackSize - 1 - 2); // The stack grows down, so we have to move the stack pointer to the start of the stack
		newThread->sreg = SREG;
		
		// Insert the new thread into the queue
		newThread->next = currentThread;
		getLastThread()->next = newThread;
		
		// Prepare the stack
		// Write entry address
		uint16_t newAddress = (uint16_t) func;
		newStack[settings.stackSize - 1] = (uint8_t) newAddress; // The return address is stored in big-endian, while uint16_t is little-endian
		newStack[settings.stackSize - 2] = (uint8_t) (newAddress >> 8);
		// Decrease stackptr to simulate pushed registers
		// newThread->stackptr -= 32;
		
		// Return PID for management purposes
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
	
	// void switchThread() {
		// uint16_t *ptr;

		// ptr = (uint16_t*)(SP + RETURN_ADDRESS_OFFSET);
		
		// // Save current state
		// currentThread->sreg = SREG;
		// currentThread->address = ptr[0];
		// currentThread->stackptr = SP;
		
		// // Switch to next thread
		// currentThread = currentThread->next;
		
		// // Restore state
		// asm("cli");
		// SP = currentThread->stackptr;
		// // asm("sei");
		// *((uint16_t*)(SP + RETURN_ADDRESS_OFFSET)) = currentThread->address;
		// SREG = currentThread->sreg;		
	// }
	
	void switchThread() {
		// Save state of current thread
		currentThread->sreg = SREG;
		currentThread->stackptr = SP;
		
		// Switch pointer
		currentThread = currentThread->next;
		
		// Load new stack pointer
		// Interrupts have to be disabled for this step
		asm("cli");
		SP = currentThread->stackptr;
		
		// Restore SREG
		SREG = currentThread->sreg;
	}
	
	void yield() {
		SM_PUSH_ALL_REGISTERS()
		Threads::switchThread();
		SM_POP_ALL_REGISTERS()
	}
}