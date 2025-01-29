#include "registers/MachRegister.h"
#include "Architecture.h"
#include <iostream>
#include <unordered_map>
#include <string>
#include <cstdlib>
#include <map>
#include <vector>
#include <algorithm>

/*
 * Ensure all registers for an architecture have unique representations
 */

// Some architectures have known aliases, so exclude them.
using dup_map = std::map<Dyninst::Architecture, std::vector<char const*>>;
dup_map known_dups = {
  {Dyninst::Arch_aarch64,
    {
      "aarch64::x29", "aarch64::fp",
      "aarch64::x30", "aarch64::lr",
      "aarch64::Ip0", "aarch64::x16",
      "aarch64::Ip1", "aarch64::x17",
    }
  },
  {Dyninst::Arch_x86, {}},
  {Dyninst::Arch_x86_64, {}},
  {Dyninst::Arch_amdgpu_gfx908, {}},
  {Dyninst::Arch_amdgpu_gfx90a, {}},
  {Dyninst::Arch_amdgpu_gfx940, {}},
};

bool check(Dyninst::Architecture arch) {
  auto const& regs = Dyninst::MachRegister::getAllRegistersForArch(arch);

  auto known_dup = [arch](std::string const& name) {
    auto const& regs = known_dups[arch];
    return std::find(regs.begin(), regs.end(), name) != regs.end();
  };

  auto success = true;

  std::unordered_map<unsigned int, std::string> vals;
  for(auto r : regs) {
    auto [itr, inserted] = vals.insert({r.val(), r.name()});
    if(!inserted && !known_dup(r.name())) {
      auto [val, name] = *itr;
      std::cerr << "Duplicate: 0x" << std::hex << val
                << "  " << r.name() << ", " << name << "\n";
      success = false;
    }
  }

  return success;
}


int main() {
  auto status = EXIT_SUCCESS;
  
  if(!check(Dyninst::Arch_aarch64)) {
    status = EXIT_FAILURE;
  }
  if(!check(Dyninst::Arch_x86)) {
    status = EXIT_FAILURE;
  }
  if(!check(Dyninst::Arch_x86_64)) {
    status = EXIT_FAILURE;
  }
  if(!check(Dyninst::Arch_ppc64)) {
    status = EXIT_FAILURE;
  }
  if(!check(Dyninst::Arch_amdgpu_gfx908)) {
    status = EXIT_FAILURE;
  }
  if(!check(Dyninst::Arch_amdgpu_gfx90a)) {
    status = EXIT_FAILURE;
  }
  if(!check(Dyninst::Arch_amdgpu_gfx940)) {
    status = EXIT_FAILURE;
  }

  return status;
}
