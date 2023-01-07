#include "CFGFactory.h"
#include "CFG.h"
#include "CFGModifier.h"
#include "CodeObject.h"
#include "CodeSource.h"
#include "InstructionAdapter.h"
#include "InstructionSource.h"
#include "Location.h"
#include "LockFreeQueue.h"
#include "ParseCallback.h"
#include "ParseContainers.h"

int main() {
	namespace dp = Dyninst::ParseAPI;
	dp::Intraproc ip;
	ip.pred_impl(nullptr);
}
