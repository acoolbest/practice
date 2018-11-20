// linear_congruential_engine::discard
#include <iostream>
#include <chrono>
#include <random>

int main ()
{
  // obtain a seed from the system clock:
//  unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	unsigned seed = 5;
  std::minstd_rand0 generator (seed);  // minstd_rand0 is a standard linear_congruential_engine
	unsigned i = 0;
	
	#if 0
  while(i++ >= 0)
  {
	  getchar();
	  std::cout << "Random value " << i << " : "<< generator() << " seed: " << seed << std::endl;
  }
  #endif
  std::cout << "Random value1: " << generator() << " seed: " << seed << std::endl;
  std::cout << "Random value2: " << generator() << " seed: " << seed << std::endl;
  generator.discard(10);     // discard 10 values

  std::cout << "Random value3: " << generator() << " seed: " << seed << std::endl;

  return 0;
}