#pragma once
#include <vector>

template<typename T>
class WalkingPool
{
	std::vector<bool> alive;
	T* data;

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
		alive.reserve(size);
		data = static_cast<T*>(malloc(size * sizeof(T)));

		for (size_t i = 0; i < size; i++)
		{
			alive.emplace_back(false);
		}
	}

	T& operator [] (size_t i)
	{
		return data[i];
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

	// ALWAYS check isFull() before calling this
	T& get()
	{
		if (freect == 0)
			return data[currentIndex];

		while (alive[currentIndex])
			incrementIndex();

		alive[currentIndex] = true;
		freect--;

		T& e = data[currentIndex];
		return e;
	}

	void release(T& e)
	{
		T* eptr = &e;
		release(eptr);
	}

	void release(T* eptr)
	{
		ptrdiff_t index = eptr - &data[0];

#ifndef NOTCOLLECTIONS_NO_DESTRUCT
		eptr->~T();
#endif
		freect++;

		alive[index] = false;
	}

	~WalkingPool()
	{
#ifndef NOTCOLLECTIONS_NO_DESTRUCT
		for (int i; i < size; i++)
			if (isAlive(i))
				data[i].~T();
#endif

		free(data);
	}
};