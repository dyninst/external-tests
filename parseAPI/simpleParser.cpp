#include "CFG.h"
#include "CodeObject.h"
#include <iostream>

namespace dp = Dyninst::ParseAPI;
int main(int argc, char* argv[]) {
  if(argc < 2 || argc > 3) {
    std::cerr << "Usage: " << argv[0] << " file\n";
    return -1;
  }

  try {
    auto* sts = new dp::SymtabCodeSource(argv[1]);
    if (!sts->getSymtabObject())  {
	std::cerr << "failed to create symtab object\n";
	return -1;
    }
    auto* co = new dp::CodeObject(sts);
    co->parse();
  } catch(std::exception &e) {
    std::cerr << e.what() << '\n';
    return -1;
  } catch(...) {
    std::cerr << "unknown exception occurred\n";
    return -1;
  }
}
