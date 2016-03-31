// InterceptorTest.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <thread>
#include <future>

extern "C" void _penter();
extern "C" void _pexit();

void x() {
	std::this_thread::sleep_for(std::chrono::seconds(1));
}

void y() {
	std::this_thread::sleep_for(std::chrono::seconds(1));
}

int main()
{
	x();
	y();
	int i = 0;
	while (i < 5) {
		std::thread t1(x);
		std::thread t2(y);
		auto f = std::async(std::launch::async, x);
		f.get();
		t1.join();
		t2.join();
		i++;
	}
    return 0;
}

