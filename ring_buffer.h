#pragma once
#include <cassert>

/// <summary>
/// Before using, you need to either allocHeap, or setBuffer.
/// </summary>
template <typename T>
class RingBuffer {
public:
	T* data{ nullptr };
	size_t head{ 0 };
	size_t tail{ 0 };

	const size_t capacity;
	size_t size{ 0 };

	bool shouldFree{ false };

	RingBuffer(size_t _capacity) : capacity(_capacity) {}

	void allocHeap() {
		data = static_cast<T*>(calloc(capacity, sizeof(T)));
		shouldFree = true;
	}

	~RingBuffer() {
		if constexpr (std::is_destructible<T>())
			for (size_t i = 0; i < size; i++)
				(*this)[i].~T();

		if (shouldFree)
			free(data);
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

	class iterator {
	private:
		T* ptr;
		const T* wrapPoint;
		size_t index;
		const size_t capacity;

	public:
		iterator(T* _ptr, T* _wrapPoint, size_t _index, size_t _capacity) :
			ptr(_ptr),
			index(_index),
			wrapPoint(_wrapPoint),
			capacity(_capacity) {}

		// Required for range for
		iterator operator++() {
			ptr++;
			index++;

			if (ptr == wrapPoint)
				ptr -= capacity;

			return *this;
		}

		bool operator==(const iterator& other) const { return index == other.index; }
		bool operator!=(const iterator& other) const { return index != other.index; }
		T& operator*() { return *ptr; }
		const T& operator*() const { return *ptr; }

	};

	iterator begin() const { return iterator(data + tail, data + capacity, 0, capacity); }
	iterator end() const { return iterator(data + head, data + capacity, size, capacity); }
};