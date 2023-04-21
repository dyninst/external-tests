#include <cstdio>

// Just need non-trivial constructor/destructor
struct Foo {
	Foo() { puts("Foo"); }
	~Foo() { puts("~Foo"); }
};

// Make it a global variable so its ctor/dtor goes in the global ctor/dtor list
Foo f;

int main(){}
