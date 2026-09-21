#include "Symtab.h"
#include "Function.h"
#include "Variable.h"

#include <iomanip>
#include <iostream>
#include <vector>

namespace ds = Dyninst::SymtabAPI;

bool check_pc_ranges(ds::Module *, ds::Function *);
bool check_local_vars(ds::Module *, ds::Function *);

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

  bool passed = true;

  if (symtab->getAllFunctionsRef().empty()) {
    std::cerr << "No functions found in '" << filename << "'\n";
    return EXIT_FAILURE;
  }

  for (auto *f : symtab->getAllFunctionsRef()) {
    auto *mod = f->getModule();

    if (mod->fullName().empty()) {
      std::cerr << "no source file found\n";
      passed = false;
      continue;
    }

    if (!check_pc_ranges(mod, f)) {
      passed = false;
      continue;
    }

    if (!check_local_vars(mod, f)) {
      passed = false;
      continue;
    }
    break;
  }

  return passed ? EXIT_SUCCESS : EXIT_FAILURE;
}

/*
 *  Ensure local variables (including function parameters)
 *  have entries in the line map.
 */
bool check_local_vars(ds::Module *mod, ds::Function *f) {
  bool passed = true;

  auto lvars = [f]() {
    std::vector<ds::localVar *> lvars;
    f->getParams(lvars);
    f->getLocalVariables(lvars);
    return lvars;
  }();

  // Print filename
  bool printed_funcname = false;
  auto pfn = [&]() -> std::ostream & {
    if (!printed_funcname) {
      std::cerr << "\nAnalyzing function " << f->getName() << ":\n";
    }
    printed_funcname = true;
    return std::cerr;
  };

  for (auto *var : lvars) {
    auto const &var_name = var->getName();

    // compilers don't always emit line information for the implicit 'this'
    // Don't consider this an error.
    if(var_name == "this") {
      continue;
    }

    if (var->getFileName().empty()) {
      pfn() << "\tno source file found for variable '" << var_name << "'\n";
      passed = false;
      continue;
    }

    if (var->getFileName() != mod->fullName()) {
      pfn() << "\t'" << var_name << "': source file '" << var->getFileName()
            << "' does not match function's source file '" << mod->fullName() << "'.\n";
      passed = false;
      continue;
    }

    const int line = var->getLineNum();
    if (line == 0) {
      pfn() << "\tvariable '" << var_name << "' declared on line 0.\n";
      passed = false;
      continue;
    }

    if (line < 0) {
      pfn() << "\tvariable '" << var_name << "' declared on negative line number.\n";
      passed = false;
      continue;
    }
  }

  return passed;
}

/*
 *  Ensure every code range covered by a function has a corresponding
 *  entry in the line map.
 */
bool check_pc_ranges(ds::Module *mod, ds::Function *f) {
  bool passed = true;

  // Print filename
  bool printed_funcname = false;
  auto pfn = [&]() -> std::ostream & {
    if (!printed_funcname) {
      std::cerr << "\n" << f->getName() << ":\n";
    }
    printed_funcname = true;
    return std::cerr;
  };

  for (auto const &range : f->getRanges()) {
    std::vector<ds::Statement::Ptr> statements;
    for (auto pc : {range.low(), range.high()}) {
      if (!mod->getSourceLines(statements, pc)) {
        pfn() << "\tno line info for PC " << pc << "\n";
        passed = false;
        continue;
      }
    }
  }
  return passed;
}
