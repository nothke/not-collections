#pragma once

#include <cassert>

template <typename T, bool noDestruct = false>
class SwappingPool
{
public:
	const size_t size;
private:
	size_t headi{ 0 };
	T* data;

public:
	SwappingPool(size_t size)
		: size(size)
	{
		assert(size > 0);

		data = static_cast<T*>(malloc(size * sizeof(T)));
	}

	~SwappingPool()
	{
		if constexpr (!noDestruct)
		{
			for (T& t : *this)
				t.~T();
		}

		free(data);
	}

	/// <summary>
	/// Only bounds checked by entire size, not by aliveCount
	/// </summary>
	T& operator [] (size_t i)
	{
		assert(i < size); // bounds check

		return data[i];
	}

	T& get()
	{
		if (headi == size)
			return data[0];

		return data[headi++]; // increments after get
	}

	void releaseAt(size_t index)
	{
		assert(index < headi);

		if (headi == 0)
			return;

		headi--;

		if constexpr (!noDestruct)
			data[headi].~T();

		if (headi == 0)
			return;

		// put last element into the one on the index
		//data[index] = data[headi];
		memcpy(&data[index], &data[headi], sizeof(T));
	}

	void release(T& e)
	{
		release(&e);
	}

	void release(T* ePtr)
	{
		ptrdiff_t index = ePtr - data;

		assert(index >= 0);
		assert(index < headi);

		releaseAt(index);
	}

	void clear() { headi = 0; }

	inline const bool isFull() const { return headi >= size; }

	inline const size_t aliveCount() const { return headi; }

	inline size_t indexOf(T& p) const
	{
		return &p - data;
	}

	class iterator {
	public:
		iterator(T* ptr) : ptr(ptr) {}

		// Required for range for
		iterator operator++() { ++ptr; return *this; }
		bool operator==(const iterator& other) const { return ptr == other.ptr; }
		bool operator!=(const iterator& other) const { return ptr != other.ptr; }
		T& operator*() { return *ptr; }
		const T& operator*() const { return *ptr; }
	private:
		T* ptr;
	};

	iterator begin() const { return iterator(data); }
	iterator end() const { return iterator(data + headi); }
};