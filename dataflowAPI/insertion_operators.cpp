#include "SymEval.h"

#include <iostream>

namespace df = Dyninst::DataflowAPI;

int main() {
  {
    df::Constant x;
    std::cout << x;
  }
  {
    df::ROSEOperation *x;
    std::cout << *x;
  }
}
