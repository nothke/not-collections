#include <iostream>
#include <iomanip>
#include "walkingpool.h"
#include "swappingpool.h"
#include "stackvector.h"
#include "fixed_vector.h"

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


	return EXIT_SUCCESS;
}