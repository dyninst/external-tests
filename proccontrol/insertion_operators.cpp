#include "Event.h"

#include <iostream>

namespace pc = Dyninst::ProcControlAPI;

int main() {
  {
    pc::Event x{pc::EventType::Error};
    std::cout << x;
  }
}
