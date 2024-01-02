#pragma once
#include <cassert>

#define NTK_CHECK_UNINITED assert(data != nullptr && "Using an uninitialized FixedVector")

template <typename T>
class FixedVector
{
	T* data{ nullptr };
	size_t headi{ 0 };
	size_t capacity{ 0 };

public:
	FixedVector() {}

	FixedVector(size_t capacity) :
		capacity(capacity)
	{
		data = static_cast<T*>(malloc(capacity * sizeof(T)));
	}

	~FixedVector() {
		if (data != nullptr)
			free(data);
	}

	void Init(size_t capacity) {
		assert(data == nullptr && "FixedVector already initialized");

		data = static_cast<T*>(malloc(capacity * sizeof(T)));
		this->capacity = capacity;
	}

	T& operator [] (size_t i)
	{
		NTK_CHECK_UNINITED;
		assert(i < head && "Index out of range");

		return data[i];
	}

	size_t size()
	{
		return headi;
	}

	T& pushRef()
	{
		NTK_CHECK_UNINITED;
		assert(!isFull() && "Getting from full FixedVector");

		if (headi == capacity)
			return data[0];

		return data[headi++];
	}

	T& insertRef(size_t at)
	{
		NTK_CHECK_UNINITED;
		assert(!isFull() && "Getting from full FixedVector");

		std::memmove(&data[at + 1], &data[at], headi - at);

		headi++;
		return data[at];
	}

	T& push(const T& t)
	{
		NTK_CHECK_UNINITED;

		T& pos = data[headi++];
		pos = t;

		return pos;
	}

	T pop()
	{
		NTK_CHECK_UNINITED;
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

#undef NTK_CHECK_UNINITED