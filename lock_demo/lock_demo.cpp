#include <iostream>
#include <thread>
#include <mutex>
#include <chrono>

std::mutex foo, bar;

void task_a(){
	std::lock(foo, bar);
	std::this_thread::sleep_for(std::chrono::seconds(2));
	std::cout <<"task a\n";
	foo.unlock();
	std::cout <<"foo unlock\n";
	std::this_thread::sleep_for(std::chrono::seconds(2));
	bar.unlock();
	std::cout <<"bar unlock\n";
}

void task_b(){
	std::this_thread::sleep_for(std::chrono::seconds(1));
//	foo.lock();
//	std::cout <<"foo lock\n";

//	bar.lock();
//	std::cout <<"bar lock\n";

	std::lock(bar, foo);
//	std::this_thread::sleep_for(std::chrono::seconds(1));
	std::cout <<"task b\n";
	foo.unlock();
	bar.unlock();
}


int main(int argc, char * argv[])
{
	std::thread th1(task_a);
	std::thread th2(task_b);

	th1.join();
	th2.join();
	return 0;
}
