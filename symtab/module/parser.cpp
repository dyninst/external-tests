#include "Module.h"
#include "Symtab.h"
#include <boost/filesystem/path.hpp>
#include <iostream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace st = Dyninst::SymtabAPI;
namespace bf = boost::filesystem;

int main() {
  st::Symtab *obj{};
  if (!st::Symtab::openFile(obj, "libmoduletest.so")) {
    std::cerr << "Unable to open 'libmoduletest.so'\n";
    return -1;
  }

  constexpr auto num_expected = 4;

  std::vector<st::Module *> modules;
  obj->getAllModules(modules);

  if (modules.size() != num_expected) {
    std::cerr << "Found " << modules.size() << " modules, but expected "
              << num_expected << '\n';
    return -1;
  }

  std::unordered_set<std::string> expected{"lib1.cpp", "lib2.cpp",
                                           "libmoduletest.so"};

  for (auto *m : modules) {
    auto const &basename = bf::path(m->fileName()).stem();
    auto const &extension = bf::path(m->fileName()).extension();
    auto const &name = basename.string() + extension.string();
    if (expected.count(name) == 0) {
      std::cerr << "Module '" << m->fileName() << "' not found.\n";
      return -1;
    }
  }

  std::vector<st::Function *> funcs;
  obj->getAllFunctions(funcs);

  std::unordered_map<std::string, std::string> funcs_by_mod{
      {"lib1func1", "lib1.cpp"},
      {"lib1func2", "lib1.cpp"},
      {"lib2func1", "lib2.cpp"},
      {"lib2func2", "lib2.cpp"},
  };

  auto find_func = [&funcs](std::string const &name) -> st::Function* {
    auto i = std::find_if(funcs.begin(), funcs.end(), [&name](st::Function *f) {
      return f->getName() == name;
    });
    if (i != funcs.end())
      return *i;
    return nullptr;
  };

  auto find_mod = [&funcs_by_mod](st::Function *f, std::string const &name) {
    auto *m = f->getModule();
    auto const &basename = bf::path(m->fileName()).stem();
    auto const &extension = bf::path(m->fileName()).extension();
    auto const &file = basename.string() + extension.string();
    return file == name;
  };

  for (auto const &fm : funcs_by_mod) {
    auto *f = find_func(fm.first);
    if (!f) {
      std::cerr << "symtab doesn't contain function '" << fm.first << "'\n";
      return -1;
    }
    if (!find_mod(f, fm.second)) {
      std::cerr << "Module '" << fm.second << "' doesn't contain function '"
                << fm.first << "'\n";
      return -1;
    }
  }
}
