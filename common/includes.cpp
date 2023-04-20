#include "Annotatable.h"
#include "Buffer.h"
#include "compiler_annotations.h"
#include "compiler_diagnostics.h"
#include "concurrent.h"
#include "DynAST.h"
#include "dyninstversion.h"
#include "dyn_regs.h"
#include "dyn_syscalls.h"
#include "dyntypes.h"
#include "Edge.h"
#include "entryIDs.h"
#include "Graph.h"
#include "IBSTree-fast.h"
#include "IBSTree.h"
#include "MachSyscall.h"
#include "Node.h"
#include "ProcReader.h"
#include "SymReader.h"
#include "util.h"
#include "VariableLocation.h"

int main() {
	Dyninst::AnnotationClass<int> ac{"string"};
	ac.getID();
}
