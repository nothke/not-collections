#pragma once
#include <cassert>

template <typename T, size_t capacity>
class StackVector
{
	T data[capacity];
	size_t headi{ 0 };

public:
	T& operator [] (size_t i)
	{
		return data[i];
	}

	size_t size()
	{
		return headi;
	}

	T& get()
	{
		if (headi == capacity)
			return data[0];

		return data[headi++];
	}

	//T& getAt()
	//T& remove()

	T& push(T t)
	{
		assert(headi < capacity - 1);

		T& pos = data[headi++];
		pos = t;

		return pos;
	}

	T pop()
	{
		assert(headi == 0);

		return data[headi--];
	}

	const bool isFull() const
	{
		return headi >= capacity;
	}

	const bool isEmpty() const
	{
		return headi == 0;
	}

	size_t indexOf(T& p) const
	{
		return &p - &data[0];
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

	iterator begin() { return iterator(&data[0]); }
	iterator end() { return iterator(&data[0] + headi); }
};