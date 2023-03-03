#include <iostream>
#include <vector>

#include <Symtab.h>
#include <Function.h>
#include <Type.h>
#include <Variable.h>

int main(int argc, char **argv) {
  if (argc < 2) {
    std::cerr << "Usage: " << argv[0] << " library.so [library.so ...]\n";
    return -1;
  }

  namespace st = Dyninst::SymtabAPI;

  for (int i = 1; i < argc; i++) {
    st::Symtab *obj = nullptr;
    auto *filename = argv[i];
    bool err = st::Symtab::openFile(obj, filename);
    if (err == false) {
      std::cerr << "Symtab::openFile failed for " << filename << '\n';
      return -1;
    }

    std::vector<st::Function *> funcs;
    if (!obj->getAllFunctions(funcs)) {
      std::cerr << "Couldn't find any functions in " << filename << '\n';
          return -1;
    }

    for (auto i : funcs) {
      std::vector<st::Symbol *> syms;
      i->getSymbols(syms);
      bool global = false;
      for (auto j : syms) {
        if (j->getLinkage() == st::Symbol::SL_GLOBAL ||
            j->getLinkage() == st::Symbol::SL_WEAK) {
          global = true;
          break;
        }
      }
      if (global == false) {
        // We don't care about internal functions right now.
        continue;
      }

      // iterate through all the parameters
      std::vector<st::localVar *> lvars;
      if (i->getParams(lvars) == false) {
        std::cerr << "Func: " << i->getName() << ": "
                  << obj->printError(obj->getLastSymtabError()) << '\n';
        continue;
      }
    }
  }
}
