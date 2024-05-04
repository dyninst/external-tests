#include "BPatch.h"

#include "BPatch_addressSpace.h"
#include "BPatch_basicBlock.h"
#include "BPatch_basicBlockLoop.h"
#include "BPatch_binaryEdit.h"
#include "BPatch_callbacks.h"
#include "BPatch_edge.h"
#include "BPatch_enums.h"
#include "BPatch_flowGraph.h"
#include "BPatch_frame.h"
#include "BPatch_function.h"
#include "BPatch_image.h"
#include "BPatch_instruction.h"
#include "BPatch_loopTreeNode.h"
#include "BPatch_memoryAccess_NP.h"
#include "BPatch_module.h"
#include "BPatch_object.h"
#include "BPatch_parRegion.h"
#include "BPatch_point.h"
#include "BPatch_process.h"
#include "BPatch_Set.h"
#include "BPatch_snippet.h"
#include "BPatch_sourceBlock.h"
#include "BPatch_sourceObj.h"
#include "BPatch_statement.h"
#include "BPatch_thread.h"
#include "BPatch_type.h"
#include "BPatch_Vector.h"
#include "StackMod.h"

int main() {
	BPatch_addressSpace *adrs;
	adrs->findOrCreateBPFunc(nullptr,nullptr);
}
