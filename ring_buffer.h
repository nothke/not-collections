#pragma once
#include <cassert>
#include <malloc.h>

template <typename T>
class RingBuffer {
public:
	T* data{ nullptr };
	size_t head{ 0 };
	size_t tail{ 0 };

	size_t capacity{ 0 };
	size_t size{ 0 };

	void allocHeap(size_t _capacity) {
		capacity = _capacity;
		data = static_cast<T*>(calloc(capacity, sizeof(T)));
	}

	void setBuffer(T* buffer) {
		data = buffer;
	}

	T& pushBack(const T& t) {
		if (tail == head && size != 0)
		{
			if (tail == capacity - 1)
				tail = 0;
			else
				tail++;
		}
		else
			size++;

		data[head] = t;

		if (head == capacity - 1)
			head = 0;
		else
			head++;

		return data[head - 1];
	}

	T& pushFront(const T& t) {
		if (tail == head && size != 0)
		{
			if (head == 0)
				head = capacity - 1;
			else
				head--;
		}
		else
			size++;

		if (tail == 0)
			tail = capacity - 1;
		else
			tail--;

		data[tail] = t;

		return data[tail];
	}

	T popBack() {
		assert(!isEmpty());

		size--;

		if (head == 0)
			head = capacity - 1;
		else
			head--;

		return data[head];
	}

	T popFront() {
		assert(!isEmpty());

		size--;

		size_t oldTail = tail;

		if (tail == capacity - 1)
			tail = 0;
		else
			tail++;

		return data[oldTail];
	}

	bool isEmpty() const { return size == 0; }
	bool isFull() const { return size >= capacity; }

	const T& operator [] (size_t i) const
	{
		assert(i < size); // bounds check

		return data[(tail + i) % capacity];
	}
};