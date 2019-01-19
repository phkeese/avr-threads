# Manual
### 1. Setup
To use Threads, you have to include `threads.hpp"` in your source file and have `stack_magic.hpp` in the same directory. Before calling `Threads::yield` the first time, you have to call `Threads::init(stackSize)` with `stackSize` being the maximum number of bytes each thread should get for its stack. Upon yielding, a thread stores all registers (r0 - r31) including the SREG on the stack. Also, upon creating a thread, the function address of the thread and the final exit loop are stored on the stack, so you will have to assign a stack size of at least 32 + 1 + 4 on CPUs with 16 bit program counter.
Calling `Threads::yield` without calling `Threads::init` first might cause some unexpected behavior and lead to crashes.

Your `main` should look something like this:
```
int main(void) {
    // Set up any peripherals
    Threads::init(128);
    // Create threads
    
    while (1) {
        // Do some stuff
        Threads::yield();
    }
}
```
### 2. Creating threads
To create executable threads, you have to call `Threads::createThread(func)`. This function will allocate all necessary memory and return a PID, which can be used to destroy the thread later. The function provided should look like this:
```
THREAD myThread(void) {
    while (gotWork()) {
        // Do some work
        Threads::yield();
    }
    // Send signal that this thread has finished
}
```
The macro `THREAD` expands to `__attribute((used)) void` to ensure that the function does not get optimized out and does not return anything. The function cannot take any arguments and should tell a manager thread that it has finished its task.  
The function may return, but it will end up in an infinite loop, until it gets destroyed from within another thread.
### 3. Switching between threads
To switch from one thread to another, call `Threads::yield`. This will automatically save all registers, including the SREG and switch out the stack pointer. Currently, all threads are dynamically allocated and contain a pointer to the next thread in the queue. Upon switching threads, the next thread in the queue will get execution time. The last thread in the queue points back at the first thread. Remember, Threads is a *cooperative* multithreading library, so you should try to call `Threads::yield` often and not hog the CPU.

### 4. Destroying threads
To destroy another thread, call `Threads::destroyThread(pid)`. This will free allocated memory and exclude the thread from the queue. ***Important: You cannot destroy a thread from within, you will have to use another thread. Calling `Threads::destroyThread` with the current PID will not do anything.***

### 5. Communicating between threads
Due to the way registers are saved and restored upon calling `Threads::yield`, variables stored in a register will not be consistent between threads. To share a value between threads, declare it as `volatile` and maybe use an IO register (perhaps `GPIOR0` on the ATmega328p) as a mutex.
