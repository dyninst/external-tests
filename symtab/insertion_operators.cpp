#include "Symtab.h"
#include "Symbol.h"
#include "Aggregate.h"
#include "Function.h"
#include "Module.h"
#include "StringTable.h"
#include "Variable.h"

#include <iostream>

namespace st = Dyninst::SymtabAPI;

int main() {
  {
    st::ModRange x;
    std::cout << x;
  }
  {
    st::Module *x;
    std::cout << *x;
  }
  {
    st::Statement x;
    std::cout << x;
  }
  {
    st::StringTable x;
    std::cout << x;
  }
  {
    st::StringTableEntry *x;
    std::cout << *x;
  }
  {
    st::Symbol x;
    std::cout << x;
  }
  {
    st::Variable x;
    std::cout << x;
  }
  {
    st::relocationEntry x;
    std::cout << x;
  }
  {
    st::Aggregate *x;
    std::cout << *x;
  }
  {
    st::Function x;
    std::cout << x;
  }
  {
    st::Variable x;
    std::cout << x;
  }
  {
    st::ExceptionBlock x;
    std::cout << x;
  }
}
