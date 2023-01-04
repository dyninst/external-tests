#include "Symtab.h"
#include "AddrLookup.h"
#include "Aggregate.h"
#include "Archive.h"
#include "Collections.h"
#include "Function.h"
#include "LineInformation.h"
#include "Module.h"
#include "RangeLookup.h"
#include "Region.h"
#include "StringTable.h"
#include "Symbol.h"
#include "SymtabReader.h"
#include "symutil.h"
#include "Type.h"
#include "Variable.h"

int main() {
	namespace ds = Dyninst::SymtabAPI;
	auto *al = ds::AddressLookup::createAddressLookup();
}
