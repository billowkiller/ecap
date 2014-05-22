/*
 * =====================================================================================
 *
 *       Filename:  test.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  05/22/2014 03:04:31 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  billowkiller (), billowkiller@gmail.com
 *   Organization:  
 *
 * =====================================================================================
 */

#ifndef ECAP_ADAPTER_STRMATCHER_H
#define ECAP_ADAPTER_STRMATCHER_H

#include <functional>
#include <chrono>
#include <future>

namespace EventTimer {
	
	class Timer {
	public:
		template <class callable, class... arguments>
		Timer(int after, bool async, callable&& f, arguments&&... args)
		{
			std::function<typename std::result_of<callable(arguments...)>::type()> task(std::bind(std::forward<callable>(f), std::forward<arguments>(args)...));

			if (async)
			{
				std::thread([after, task]() {
					std::this_thread::sleep_for(std::chrono::seconds(after));
					task();
				}).detach();
			}
			else
			{
				std::this_thread::sleep_for(std::chrono::seconds(after));
				task();
			}
		}
		
		template <class callable>
		Timer(int after, bool async, callable& f)
		{

			if (async)
			{
				std::thread([after, f]() {
					std::this_thread::sleep_for(std::chrono::seconds(after));
					f();
				}).detach();
			}
			else
			{
				std::this_thread::sleep_for(std::chrono::seconds(after));
				f();
			}
		}
	};
	
// class A
// {
// 	int a;
// public:
// 	A(int x):a(x){}
// 	void print(){ std::cout << a << std::endl; }
// };

}

#endif

// void test1(void)
// {
//     return;
// }
// 
// void test2(int a)
// {
//     std::cout << a << std::endl;
//     exit(0);
//     return;
// }
// 
// int main()
// {
// 	A aa(2);
// 	std::function<void()> f1; 
// 	f1 = std::bind(&A::print, &aa);
//     Timer later_test2(1, false , f1);
// 	Timer later_test2(1, false , &test2, 22);
//     return 0;
// }
