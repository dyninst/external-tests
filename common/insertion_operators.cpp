#include "IBSTree-fast.h"
#include "IBSTree.h"

#include <iostream>

int main() {
  struct Foo {
    using type = int;
    int high() const { return 1; }
  };

  {
    Dyninst::IBSNode<Foo> x;
    std::cout << x;
  }
  {
    Dyninst::IBSTree<Foo> x;
    std::cout << x;
  }
  {
    Dyninst::IBSTree_fast<Foo> x;
    std::cout << x;
  }
}
