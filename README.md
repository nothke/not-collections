# not-collections
Simple, header-only, high performance, templated collections

Made specifically for my own game development uses, but can be used in any other scenario.

There are no inter-dependencies, you can just add whichever you like to your project.

Note that this repo is being made while still learning C++, so take note that I'm not an expert, and probably some things could've been done a bit better.

## What's the point?
Using malloc/new (heap memory allocations) can be very time consuming and should be minimized at runtime, during gameplay. These containers provide a way to malloc only once, a single block of memory and then reuse that block for multiple "local" allocations (called "get" and "release" in my containers).

### WhY nOt vEcToR&lt;T<>?
Vector is great, but there is multiple reasons not to use it. It is great as a general purpose container, but when you need something specific as it is the case for games, like very frequent insertions and deletions of tiny objects, or guaranteeing that the pointers to elements will not change during their lifetime, you need something more. Another very important thing is that vectors will resize when more elements are inserted than its capacity, and that is almost always a bad thing. In that case, vectors will reallocate to a new location (allocating new memory on the heap), and as a result call constructors and destructors when that was unintended, like when relying on a RAII paradigm.

Common properties of all containers:
* These allocators DO NOT [re]initialize memory by default, you'll need to call placement new manually, like `new (&pool[i]) T(args);`, or set the values without calling the constructor. The reason for this is that you might want to call a custom constructor on an element, and implementing a custom emplace_back()-like within these containers would lead to code bloat.
* The allocators manage memory in RAII way. They malloc when constructed, and they will deallocate when going out of scope.
* You'll need to check the size with `isFull()` before inserting an element to prevent going out of bounds, there is no automatic check.

## WalkingPool&lt;T>
A walking pool is a dynamic array with fixed capacity, that "walks" through the pool searching for the next empty slot. Ideal for smaller arrays of larger objects where elements are required to be fixed in place in memory during their lifetime.
* The insertion search will be O(n) in the worst case.
* Looping through the pool requires you to check if the instance `isAlive(index)` (but nothing prevents you from touching the dead ones either).
* Since the "alive" memory becomes fragmented when inner elements are destroyed, it can lead to performance losses due to missing cache lines (but is still much faster than pointing into random pieces of heap memory).
* It guarantees that instances will not change their memory location while they are alive, so it is safe to take a reference or a pointer to an element and use it elsewhere.

## SwappingPool&lt;T>
A swapping pool is a dynamic array with fixed capacity that uses last element swap-back when releasing an element, making it ideal for collections of small objects that require frequent insertions and deletions (think: particle systems).
* Insertion and release will always be O(1).
* There is no fragmentation of alive elements as all alive elements are always next to each other in the range of 0..aliveCount().
* The elements will not be at the same locations after releasing an element, so you should never keep a reference/pointer to an index between structural changes.
* Releasing involves copying an element, which might be expensive for bigger objects.
