#include <iostream>
#include <iomanip>
#include "walkingpool.h"
#include "swappingpool.h"
#include "stackvector.h"
#include "fixed_vector.h"
#include "ring_buffer.h"

struct TestStruct
{
	int a{ 1 };
	int b{ 2 };
	char c{ 25 };

	TestStruct()
	{
		std::cout << "Created!" << std::endl;
	}

	TestStruct(int a, int b) :a(a), b(b) {
		std::cout << "Created!" << std::endl;
	}

	~TestStruct()
	{
		std::cout << "Destroyed!" << std::endl;
	}

	void Debug() const
	{
		int ci = (int)c;
		std::cout << "My struct: " << a << ", " << b << ", " << (ci) << std::endl;
	}
};

struct AllInOne {
	float playerSpeed;
	int playerX;
	int playerY;

	StackVector<float, 10> values;
	StackVector<TestStruct, 128> structs;
};

namespace Random
{
	inline int Range(int from, int to)
	{
		int v = rand();
		double v01 = (double)v / RAND_MAX;
		return static_cast<int>(from + (to - from) * v01);
	}

	inline auto Value()
	{
		int v = rand();
		double v01 = (double)v / RAND_MAX;
		return v01;
	}
}

int main()
{
	/*
	{
		std::cout << "\nWALKING POOL\n\n";

		// Create a walking pool with capacity
		WalkingPool<TestStruct> pool(16);

		const auto OutputPool = [&]() {
			std::cout << "\n|";

			for (size_t i = 0; i < pool.size; i++)
			{
				if (pool.isAlive(i))
				{
					std::cout << std::setw(3) << pool[i].a;
				}
				else
					std::cout << " --";
			}

			std::cout << "|\n";
			};

		int seq = 0;

		const auto Create = [&]() {
			TestStruct& _s = pool.get();
			new (&_s) TestStruct(seq++, 42);
			OutputPool();
			};

		TestStruct& s0 = pool.get();
		// Call constructor explicitly
		new (&s0) TestStruct(seq++, 10);

		OutputPool();

		TestStruct& s1 = pool.get();
		new (&s1) TestStruct(seq++, 4);

		OutputPool();

		// Explicitely release
		pool.release(s0);
		OutputPool();
		pool.release(s1);
		OutputPool();

		// Acquire more
		for (size_t i = 0; i < 20; i++)
		{
			if (!pool.isFull())
				Create();
		}

		pool.release(pool[3]);
		OutputPool();

		Create();

		pool.clear();

		OutputPool();

		for (size_t i = 0; i < 6; i++)
		{
			Create();
		}

		std::cout << "Alive count before range for: " << pool.getAliveCount() << "\n";

		for (auto& s : pool)
		{
			s.Debug();
		}

		// "pool" deallocates here, when leaving the scope
		// releasing all elements in the process
	}

	{
		std::cout << "\nSWAPPING POOL\n\n";

		SwappingPool<TestStruct, true> pool(16);

		const auto OutputPool = [&]() {
			std::cout << "\n|";

			for (size_t i = 0; i < pool.size; i++)
			{
				if (i < pool.aliveCount())
				{
					//std::cout << " # ";
					std::cout << std::setw(3) << pool[i].a;
				}
				else
					std::cout << " - ";


			}

			std::cout << "|\n";
			};

		int seq = 0;

		TestStruct& s0 = pool.get();
		// Call constructor explicitly
		new (&s0) TestStruct(seq++, 10);

		OutputPool();

		// Kill the element so it's space can be reused, implicitly calls destructor
		pool.release(s0);

		OutputPool();

		// Allocate a few more elements
		for (size_t i = 0; i < 40; i++)
		{
			// Don't add a new one if full
			if (!pool.isFull())
			{
				TestStruct& s1 = pool.get();
				new (&s1) TestStruct(seq++, 10);

			}

			OutputPool();
		}

		for (size_t i = 0; i < 4; i++)
		{
			pool.release(pool[i]);
			OutputPool();
		}

		OutputPool();

		auto& last = pool[pool.aliveCount() - 1];
		pool.release(last);

		std::cout << "Released last element:\n";
		OutputPool();

		for (size_t i = 0; i < 20; i++)
		{
			if (Random::Value() < 0.5)
			{
				auto& s = pool.get();
				s.a = seq++;
			}
			else
			{
				int killi = Random::Range(0, pool.aliveCount());
				std::cout << "Killing at: " << killi << "\n";
				pool.release(pool[killi]);
			}

			if (Random::Value() < 0.1)
				pool.clear();

			OutputPool();
		}

		// loop through the pool with range for
		for (auto& s : pool)
		{
			s.Debug();
		}

		// "pool" deallocates here, when leaving the scope
	}

	{
		std::cout << "\nSTACK VECTOR\n\n";

		StackVector<TestStruct, 8> stackVec;

		stackVec.push(TestStruct(10, 10));
		auto& pos = stackVec.get();
		pos.a = 10;

		std::cout << "size: " << stackVec.size() << "\n";

		for (auto& s : stackVec)
		{
			s.Debug();
		}

		std::cout << "\nFIXED VECTOR\n\n";

		FixedVector<TestStruct> fixedVec(8);

		std::cout << "-- created\n";

		auto& loc = fixedVec.pushRef();
		new (&loc) TestStruct(9, 9);

		//#define INPLACE(x, init) new (&x) init

		auto& loc2 = fixedVec.insertRef(0);
		loc2.a = 777;
		loc2.b = 888;
		loc2.c = 'c';

		std::cout << "-- after push\n";

		for (size_t i = 0; i < 4; i++)
		{
			fixedVec.push(TestStruct(4 + i, 5 + i));
		}

		for (auto& s : fixedVec)
		{
			s.Debug();
		}

		std::cout << "Uninited FixedVector\n";
		FixedVector<TestStruct> fixedVec2;

		fixedVec2.Init(64);

		fixedVec2.push({});

		std::cout << "\nSTD VECTOR\n\n";

		std::vector<TestStruct> stdVec{};
		stdVec.reserve(16);

		stdVec.push_back({});
		std::cout << "^ push_back\n";

		stdVec.emplace_back(1, 2);
		std::cout << "^ emplace_back\n";

		// Excpected assert fail
		//for (size_t i = 0; i < 20; i++)
		//	fixedVec.push({});

		std::cout << "\nAllInOne\n";
		//auto& all = *new AllInOne();
		auto& all = *static_cast<AllInOne*>(malloc(sizeof(AllInOne)));

		std::cout << "\nCreated AllInOne\n";

		std::cout << sizeof(all);

		std::cout << "\n\n---- END ----\n\n";

		free(&all);
		//delete& all;
	}
	*/

#if false
	{
		std::cout << "\nRING BUFFER\n\n";

		RingBuffer<int> buffer;

		const auto OutputRingBuffer = [&]() {
			std::cout << "\n|";

			for (size_t i = 0; i < buffer.capacity; i++)
			{
				std::cout << std::setw(3);

				if (buffer.tail == i)
					std::cout << "T";

				if (buffer.head == i)
					std::cout << "H";

				if (buffer.tail != i && buffer.head != i)
					std::cout << " ";

				std::cout << " :";
			}

			std::cout << "|\n|";

			for (size_t i = 0; i < buffer.capacity; i++)
			{
				std::cout << std::setw(3) << buffer.data[i];
				std::cout << " :";
			}

			std::cout << "|\n";

			std::cout << "size: " << buffer.size << "\n";
			};

		buffer.allocHeap(64);

		int a = 1;

		// Random pushes and pops from both sides

		/*
		for (size_t i = 0; i < 100; i++)
		{
			int randi = Random::Range(0, 4);

			if (randi == 0)
			{
				std::cout << "pushBack" << "\n";
				buffer.pushBack(a++);
			}
			else if (randi == 1)
			{
				std::cout << "pushFront" << "\n";
				buffer.pushFront(a++);
			}
			else
			{
				if (!buffer.isEmpty())
				{
					if (randi == 2)
					{
						std::cout << "popBack" << "\n";
						buffer.popBack();
					}
					else if (randi == 3)
					{
						std::cout << "popFront" << "\n";
						buffer.popFront();
					}
				}
			}

			OutputRingBuffer();
		}

		buffer.pushBack(a++);

		OutputRingBuffer();

		for (size_t i = 0; i < buffer.size; i++)
		{
			std::cout << buffer[i] << "\n";
		}
		*/


		for (size_t i = 0; i < 1000; i++)
		{
			buffer.pushBack(a++);
		}

		OutputRingBuffer();

		std::cout << "\nnormal for:\n";

		for (size_t i = 0; i < buffer.size; i++)
		{
			std::cout << buffer[i] << "\n";
		}

		std::cout << "\nrange for:\n";

		for (auto& i : buffer)
		{
			std::cout << i << "\n";
		}

		/*
		buffer.pushBack(a++);

		for (int i = 0; i < 4; i++)
		{
			buffer.pushBack(a++);
			OutputRingBuffer();
		}

		for (size_t i = 0; i < 4; i++)
		{
			buffer.popBack();
			OutputRingBuffer();
		}

		for (size_t i = 0; i < 10; i++)
		{
			buffer.pushFront(100 - i);
			OutputRingBuffer();
		}

		buffer.pushFront(666);
		OutputRingBuffer();


		for (size_t i = 0; i < 4; i++)
		{
			buffer.popFront();
			OutputRingBuffer();
		}
		*/


		/*
		OutputRingBuffer();

		for (int i = 0; i < 10; i++)
		{
			buffer.pushBack(i);
			OutputRingBuffer();
		}

		buffer.popBack();
		OutputRingBuffer();
		buffer.popBack();
		OutputRingBuffer();
		buffer.popBack();
		OutputRingBuffer();
		buffer.popBack();
		OutputRingBuffer();

		for (int i = 0; i < 10; i++)
		{
			buffer.pushBack(i);
			OutputRingBuffer();
		}

		buffer.pushFront(99);
		OutputRingBuffer();
		buffer.pushFront(98);
		OutputRingBuffer();
		buffer.pushFront(97);
		OutputRingBuffer();
		buffer.pushFront(96);
		OutputRingBuffer();
		buffer.pushFront(95);
		OutputRingBuffer();
		buffer.pushFront(94);
		OutputRingBuffer();
		*/
	}
#endif

	{
		//RingBuffer<TestStruct> buffer(32);
		//buffer.allocHeap();

		//auto& a = buffer.pushBack({});
		//a.a = 100;

		//a.Debug();
	}

	{
		std::cout << "\nRingBuffer ctor/dtor:\n";

		RingBuffer<TestStruct> buffer(4);
		char buf[4 * sizeof(TestStruct)];
		buffer.setBuffer(reinterpret_cast<TestStruct*>(buf));

		auto& a = buffer.pushBack({});
		a.a = 100;
		TestStruct temp;
		buffer.pushBack(temp);
		buffer.pushBack(temp);
		buffer.pushBack(temp);
		buffer.pushBack(temp);
		buffer.pushBack(temp);
		buffer.pushBack(temp);

		for (auto& s : buffer)
		{
			s.Debug();
		}
	}

	{
		std::cout << "\nvector ctor/dtor:\n";

		std::vector<TestStruct> vec;
		vec.reserve(12);

		vec.emplace_back();
		vec.emplace_back();
		vec.emplace_back();
		vec.emplace_back();

		for (auto& s : vec)
		{
			s.Debug();
		}
	}

	return EXIT_SUCCESS;
}