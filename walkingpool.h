#pragma once
#include <vector>

template<typename T>
class WalkingPool
{
	std::vector<bool> alive;
	std::vector<T> entities;

	size_t currentIndex;
	size_t freect;

public:
	const size_t size;

private:
	inline void incrementIndex()
	{
		currentIndex++;
		if (currentIndex >= size)
			currentIndex = 0;
	}

public:
	WalkingPool(const size_t size)
		:currentIndex(0), freect(size), size(size)
	{
		entities.reserve(size);
		alive.reserve(size);

		for (size_t i = 0; i < size; i++)
		{
			entities.emplace_back();
			alive.emplace_back(false);
		}
	}

	T& operator [] (size_t i)
	{
		return entities[i];
	}

	inline const bool isFull() const
	{
		return freect == 0;
	}

	inline const bool isAlive(size_t index) const
	{
		return alive[index];
	}

	inline const int getAliveCount() const
	{
		return size - freect;
	}

	// ALWAYS use hasSpace() before calling this
	T& get()
	{
		if (freect == 0)
			return entities[currentIndex];

		while (alive[currentIndex])
			incrementIndex();

		alive[currentIndex] = true;
		freect--;

		Entity& e = entities[currentIndex];
		return e;
	}

	// returns nullptr if pool is full
	T* getAsPtr()
	{
		if (freect == 0)
			return nullptr;

		while (alive[currentIndex])
			incrementIndex();

		alive[currentIndex] = true;
		freect--;

		return &entities[currentIndex];
	}

	void release(T& e)
	{
		T* eptr = &e;
		release(eptr);
	}

	void release(T* eptr)
	{
		ptrdiff_t index = eptr - &entities[0];

		//eptr->~T();
		freect++;

		alive[index] = false;
	}
};