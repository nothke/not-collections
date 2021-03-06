//#define NOTCOLLECTIONS_NO_DESTRUCT

#pragma once
template <typename T>
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
		data = static_cast<T*>(malloc(size * sizeof(T)));
	}

	T& operator [] (size_t i)
	{
		return data[i];
	}

	T& get()
	{
		if (headi == size)
			return data[0];

		return data[headi++]; // increments after get
	}

	void release(T& e)
	{
		release(&e);
	}

	void release(T* eptr)
	{
		if (headi == 0)
			return;

		headi--;
#ifndef NOTCOLLECTIONS_NO_DESTRUCT
		data[headi].~T();
#endif

		if (headi == 0)
			return;

		ptrdiff_t index = eptr - &data[0];

		// put last element into the one on the index
		//data[index] = data[headi];
		memcpy(&data[index], &data[headi], sizeof(T));
	}

	const bool isFull() const
	{
		return headi >= size;
	}

	const size_t aliveCount() const { return headi; }

	~SwappingPool()
	{
#ifndef NOTCOLLECTIONS_NO_DESTRUCT
		for (T& t : *this)
			t.~T();
#endif

		free(data);
	}

	class iterator {
	public:
		iterator(T* ptr) : ptr(ptr) {}
		iterator operator++() { ++ptr; return *this; }
		bool operator!=(const iterator& other) const { return ptr != other.ptr; }
		T& operator*() { return *ptr; }
		const T& operator*() const { return *ptr; }
	private:
		T* ptr;
	};

	iterator begin() const { return iterator(data); }
	iterator end() const { return iterator(data + headi); }
};