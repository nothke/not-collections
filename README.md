# not-collections
Simple, header-only, high performance, templated collections

Made specifically for my own game development uses, but can be used in any other scenario.

There are no inter-dependencies, you can just add whichever you like to your project.

Note that this repo is being made while still learning C++, so take note that I'm not a genius, and probably some things could've been done a bit better.

Common properties of all containers:
* These allocators DO NOT [re]initialize memory by default, you'll need to call placement new manually, like `new &pool[i] T(args);`. The reason for this is that you might want to call a custom constructor on an element, and implementing a custom emplace_back()-like within these containers would lead to code bloat.
* The allocators manage memory in RAII way. They malloc when constructed, and they will deallocate when going out of scope.
* You'll need to check the size with `isFull()` before inserting an element to prevent going out of bounds, there is no automatic check.

## WalkingPool&lt;T>
A walking pool is a dynamic array with fixed capacity, that "walks" through the pool searching for the next empty slot.
* The insertion search will be O(n) in the worst case.
* Looping through the pool requires you to check if the instance `isAlive(index)` (but nothing prevents you from touching the dead ones either).
* It guarantees that instances will not change their memory location while they are alive, so it is safe to take a reference or a pointer to an element.

## SwappingPool&lt;T>
A swapping pool is a dynamic array with fixed capacity that uses last element swap-back when releasing an element, making it ideal for collections of small objects that require frequent insertions and deletions (think: particle systems).
* Insertion and release will always be O(1).
* There is no fragmentation of alive elements as all alive elements are always next to each other in the range of 0..aliveCount().
* The elements will not be at the same locations after releasing an element, so you should never keep a reference/pointer to an index between structural changes.
* Releasing involves copying an element, which might be expensive for bigger objects.
