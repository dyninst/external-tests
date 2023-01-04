#include "SymLite-elf.h"

int main() {
	Dyninst::SymElf *e;
	e->getSymbolByName("foo");
}
