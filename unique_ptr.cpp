/*
 *  g++ unique_ptr.cpp -g -std=c++14
 */
#include <iostream>
#include <vector>
#include <memory>
#include <cstdio>
#include <fstream>
#include <cassert>
#include <functional>
 
struct B {
  virtual void bar() { std::cout << "B::bar\n"; }
  virtual ~B() = default;
};
struct D : B
{
    D() { std::cout << "D::D\n";  }
    ~D() { std::cout << "D::~D\n";  }
    void bar() override { std::cout << "D::bar\n";  }
};
 
// a function consuming a unique_ptr can take it by value or by rvalue reference
std::unique_ptr<D> pass_through(std::unique_ptr<D> p)
{
	std::cout << "pass_through begin\n";
    p->bar();
	std::cout << "pass_through end\n";
    return p;
}
 
int main()
{
  std::cout << "unique ownership semantics demo\n";
  {
      auto p = std::make_unique<D>(); // p is a unique_ptr that owns a D
      std::cout << "1\n";
	  auto q = pass_through(std::move(p)); 
      std::cout << "2\n";
	  assert(!p); // now p owns nothing and holds a null pointer
      std::cout << "3\n";
	  q->bar();   // and q owns the D object
	  std::cout << "4\n";
  } // ~D called here
 
  std::cout << "Runtime polymorphism demo\n";
  {
    std::unique_ptr<B> p = std::make_unique<D>(); // p is a unique_ptr that owns a D
                                                  // as a pointer to base
      std::cout << "1\n";
    p->bar(); // virtual dispatch
 
      std::cout << "2\n";
    std::vector<std::unique_ptr<B>> v;  // unique_ptr can be stored in a container
      std::cout << "3\n";
    v.push_back(std::make_unique<D>());
      std::cout << "4\n";
    v.push_back(std::move(p));
      std::cout << "5\n";
    v.emplace_back(new D);
      std::cout << "6\n";
    for(auto& p: v) p->bar(); // virtual dispatch
      std::cout << "7\n";
  } // ~D called 3 times
 
  std::cout << "Custom deleter demo\n";
  std::ofstream("demo.txt") << 'x'; // prepare the file to read
  {
      std::unique_ptr<std::FILE, decltype(&std::fclose)> fp(std::fopen("demo.txt", "r"),
                                                            &std::fclose);
      std::cout << "1\n";
      if(fp) // fopen could have failed; in which case fp holds a null pointer
        std::cout << (char)std::fgetc(fp.get()) << '\n';
      std::cout << "2\n";
  } // fclose() called here, but only if FILE* is not a null pointer
    // (that is, if fopen succeeded)
 
  std::cout << "Custom lambda-expression deleter demo\n";
  {
    std::unique_ptr<D, std::function<void(D*)>> p(new D, [](D* ptr)
        {
            std::cout << "destroying from a custom deleter...\n";
            delete ptr;
        });  // p owns D
    p->bar();
  } // the lambda above is called and D is destroyed
 
  std::cout << "Array form of unique_ptr demo\n";
  {
      std::unique_ptr<D[]> p{new D[3]};
  } // calls ~D 3 times
}
