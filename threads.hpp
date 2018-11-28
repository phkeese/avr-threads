#pragma once

#include <stdint.h>
#include "stack_magic.hpp"
#define THREAD __attribute((used)) void

namespace Threads {
	typedef struct Thread Thread;
	struct Thread {
		uint16_t pid;
		uint16_t sreg;
		uint16_t stackptr;
		uint16_t stackbase;
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
	__attribute((noinline)) void switchThread();
	
	// Primary management functions
	__attribute((noinline)) void init(uint16_t stackSize);
	__attribute((noinline)) uint16_t createThread(void (*func)(void));
	void yield();
}