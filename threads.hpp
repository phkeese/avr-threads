#pragma once

#include <stdint.h>
#include "stack_magic.hpp"
#define THREAD __attribute((used)) void

namespace Threads {
	typedef struct Thread Thread;
	struct Thread {
		uint16_t pid;
		uint16_t stackptr;
		uint8_t *stackbase;
		Thread *next;
	};

	extern struct Settings {
		uint16_t stackSize;
	} settings;

	// Book keeping values
	extern Thread *currentThread;
	extern uint16_t nextAddress;
	extern uint16_t tmpAddress;
	extern uint8_t high,low;

	// Secondary management functions
	uint16_t getNextPID();
	Thread *getLastThread();
	uint8_t *initStack(uint8_t* stackbase);
	
	// Primary management functions
	__attribute((noinline)) void init(uint16_t stackSize);
	__attribute((noinline)) uint16_t createThread(void (*func)(void));
	__attribute((noinline)) void yield();
	
	__attribute((noinline)) void exit(void);
}