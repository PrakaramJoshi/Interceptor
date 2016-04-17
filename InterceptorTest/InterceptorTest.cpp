// InterceptorTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <thread>
#include <future>
#include <iostream>
extern "C" void _penter();
extern "C" void _pexit();
namespace Test {
	struct A {
		void a() {
			std::this_thread::sleep_for(std::chrono::seconds(1));
		}
	};
}
void x() {
	std::this_thread::sleep_for(std::chrono::seconds(1));
}

void y() {
	x();
	std::this_thread::sleep_for(std::chrono::seconds(1));
}

int main()
{
	Test::A a;
	x();
	y();
	a.a();
	unsigned int u = 0;
	int i = 4;
	while (i < 5) {
		std::thread t1(x);
		std::thread t2(y);
		auto f = std::async(std::launch::async, x);
		f.get();
		t2.join();
		t1.join();
		
		i++;
	}
    return 0;
}

