#include <iostream>
#include "walkingpool.h"
#include "swappingpool.h"

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

	void Debug()
	{
		int ci = (int)c;
		std::cout << "My struct: " << a << ", " << b << ", " << (ci) << std::endl;
	}
};

int main()
{
	{
		std::cout << "\nWALKING POOL\n\n";

		// Create a walking pool with capacity
		WalkingPool<TestStruct> pool(32);

		TestStruct& s0 = pool.get();
		// Call constructor explicitly
		new (&s0) TestStruct(10, 10);

		// "pool" deallocates here, when leaving the scope
	}

	{
		std::cout << "\nSWAPPING POOL\n\n";

		SwappingPool<TestStruct> pool(32);

		TestStruct& s0 = pool.get();
		// Call constructor explicitly
		new (&s0) TestStruct(10, 10);

		// Kill the element so it's space can be reused, implicitly calls destructor
		pool.release(s0);

		// Allocate a few more elements
		for (size_t i = 0; i < 40; i++)
		{
			// Don't add a new one if full
			if (!pool.isFull())
			{
				TestStruct& s1 = pool.get();
				new (&s1) TestStruct(10, 10);
			}
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