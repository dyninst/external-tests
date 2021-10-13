#include "BPatch.h"
#include "BPatch_basicBlock.h"

#include <iostream>

int main() {
  {
    BPatch_basicBlock *x;
    std::cout << *x;
  }
}
