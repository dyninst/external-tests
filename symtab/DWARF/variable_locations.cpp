#include "Symtab.h"
#include "Function.h"
#include "Variable.h"
#include "VariableLocation.h"

#include <iostream>
#include <vector>

namespace ds = Dyninst::SymtabAPI;

static std::ostream& operator<<(std::ostream &os, Dyninst::VariableLocation const &loc) {
  return os << '[' << std::hex << loc.lowPC << ", " << std::hex << loc.hiPC << ')';
}

static std::ostream& operator<<(std::ostream &os, ds::FuncRange const &range) {
  return os << '[' << std::hex << range.low() << ", " << std::hex << range.high() << ')';
}

/*
 *  Test that all local variables and parameters have DWARF locations
 *  within the bounds of their containing function.
 */

int main(int argc, char **argv) {
  if (argc != 2) {
    std::cerr << "Usage: " << argv[0] << " file\n";
    return EXIT_FAILURE;
  }

  auto *filename = argv[1];
  ds::Symtab *symtab{};

  if (!ds::Symtab::openFile(symtab, filename)) {
    std::cerr << "Unable to open file '" << filename << "'\n";
    return EXIT_FAILURE;
  }

  bool failed = false;

  for (auto *f : symtab->getAllFunctionsRef()) {
    std::clog << "Checking function '" << f->getName() << "'\n";

    std::vector<ds::localVar *> lvars;
    f->getParams(lvars);
    f->getLocalVariables(lvars);

    for (auto *var : lvars) {
      std::clog << "Checking variable '" << var->getName() << "' ["
                << var->getFileName() << ':' << var->getLineNum() << "]\n";

      for (auto var_loc : var->getLocationLists()) {
        if (var_loc.lowPC == 0 || var_loc.hiPC >= 0xFFFFFFFF) {
          std::cerr << "Location is likely bad " << var_loc << "\n";
          failed = true;
        }
        for (auto const &func_loc : f->getRanges()) {
          if (var_loc.lowPC < func_loc.low() || var_loc.hiPC > func_loc.high()) {
            std::clog << "Outside of function range!\n"
                      << "  var = " << var_loc << "\n"
                      << "  func = " << func_loc << "\n";
            failed = true;
          }
        }
      }
    }
  }

  return failed ? EXIT_FAILURE : EXIT_SUCCESS;
}
