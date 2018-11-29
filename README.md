# Threads v0.6 development branch

Planned features:
- [ ] Optimize memory usage
- [ ] Rewrite yield and createThread, so only the stack pointer has to be changed
- [ ] Implement a way of destroying threads

Problems & Solutions:
- `createThread` seems to create an invalid entry address or...
- `initStack` does not setup the stack correctly

- Maybe actually switch to stack in `initStack`?