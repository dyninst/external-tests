#include "Symtab.h"
#include "Function.h"
#include <exception>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace st = Dyninst::SymtabAPI;

static void check_types(st::Symtab *t);
static void check_params(st::Symtab *t);

namespace {
const std::vector<std::string> type_names = {
    "ui",       "sc",   "ec_ui",   "ec_sc",   "e",  "td_ec_ui",
    "td_ec_sc", "td_e", "u_ec_ui", "u_ec_sc", "u_e"};
}

int main() {
  st::Symtab *obj{};
  if (!st::Symtab::openFile(obj, "libenum.so")) {
    std::cerr << "Unable to open 'libenum.so'\n";
    return -1;
  }

  try {
    check_types(obj);
    check_params(obj);
  } catch (std::exception const &e) {
    std::cerr << e.what() << std::endl;
    return -1;
  }

  std::cout << "OK\n";
}

static st::Type *type_of(st::Symtab *obj, std::string const &name) {
  st::Type *t{};
  obj->findType(t, name);
  return t;
}

static std::string parse(st::Type *t);

static void assert_equal(std::string const &a, std::string const &b) {
  if (a != b) {
    throw std::runtime_error{"Got '" + a + "', expected '" + b + "'"};
  }
}

void check_types(st::Symtab *obj) {
  auto underlying_type_of = [&](std::string const &name) {
    auto *t = type_of(obj, name);
    if (t->getDataClass() != st::dataEnum) {
      throw std::runtime_error{name + " is not an enum"};
    }
    auto *ct = t->asEnumType().getConstituentType();
    if (!ct) {
      throw std::runtime_error{name + " has no underyling type"};
    }
    return ct;
  };
  auto assert_underlying_type = [&](std::string const &enum_name,
                                    std::string const &ut_name) {
    auto *enum_ut = underlying_type_of(enum_name);
    auto *t = type_of(obj, ut_name);
    if (!(*enum_ut == *t)) {
      throw std::runtime_error{"underlying type of " + enum_name + "('" +
                               enum_ut->getName() + "') != '" + ut_name + "'"};
    }
  };

  // Make sure all types are accessible
  for (auto const &tn : type_names) {
    if (!type_of(obj, tn)) {
      throw std::runtime_error{"Unable to find type name '" + tn + "'"};
    }
  }

  // Check that we correctly parsed the underlying types of the scoped enums
  assert_underlying_type("ec_ui", "ui");
  assert_underlying_type("ec_sc", "sc");

  // Ensure the using aliases are equivalent to the typedefs
  assert_equal(parse(type_of(obj, "td_ec_ui")), parse(type_of(obj, "u_ec_ui")));
  assert_equal(parse(type_of(obj, "td_ec_sc")), parse(type_of(obj, "u_ec_sc")));
  assert_equal(parse(type_of(obj, "td_e")), parse(type_of(obj, "u_e")));
}

void check_params(st::Symtab *obj) {

  for (auto const &n : type_names) {
    std::vector<st::Function *> functions;
    obj->findFunctionsByName(functions, "fun_" + n, st::prettyName);
    if (functions.size() == 0) {
      throw std::runtime_error{"Could not find function fun_" + n};
    }
    if (functions.size() != 1) {
      throw std::runtime_error{"Found more than one instance of fun_" + n};
    }
    std::vector<st::localVar *> params;
    if (!functions[0]->getParams(params)) {
      throw std::runtime_error{"Could not get parameters for fun_" + n};
    }
    if (params.size() == 0) {
      throw std::runtime_error{"Found 0 params, but expected 1 for fun_" + n};
    }
    if (params.size() != 1) {
      throw std::runtime_error{"Found " + std::to_string(params.size()) +
                               " params, but expecetd 1 for fun_" + n};
    }
    auto *return_type = functions[0]->getReturnType();
    if (!return_type) {
      throw std::runtime_error{"Unable to read return type for fun_" + n};
    }

    // The parameter type must match the return type
    auto const& cur_type = parse(type_of(obj, n));
    assert_equal(cur_type, parse(params[0]->getType()));
    assert_equal(cur_type, parse(return_type));
  }
}

void parse(st::Type *t, std::ostringstream &os) {
  if (!t) {
    os << "NULL";
  }
  os << st::dataClass2Str(t->getDataClass()) << "(";
  switch (t->getDataClass()) {
  case st::dataReference: {
    parse(t->getRefType()->getConstituentType(), os);
    os << ')';
    break;
  }
  case st::dataTypedef: {
    parse(t->getTypedefType()->getConstituentType(), os);
    break;
  }
  case st::dataPointer: {
    parse(t->getPointerType()->getConstituentType(), os);
    break;
  }
  case st::dataEnum: {
    auto *e = t->getEnumType();
    if (e->is_scoped()) {
      os << "class ";
    }
    parse(e->getConstituentType(), os);
  } break;
  default:
    os << t->getName();
    break;
  }
  os << ')';
}

std::string parse(st::Type *t) {
  std::ostringstream os;
  parse(t, os);
  return os.str();
}
