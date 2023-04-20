#include "AddrSpace.h"
#include "CFGMaker.h"
#include "Command.h"
#include "Instrumenter.h"
#include "PatchCallback.h"
#include "PatchCFG.h"
#include "PatchCommon.h"
#include "PatchMgr.h"
#include "PatchModifier.h"
#include "PatchObject.h"
#include "Point.h"
#include "Snippet.h"

int main() {
	namespace dp = Dyninst::PatchAPI;
	auto *a = dp::AddrSpace::create(nullptr);
}
