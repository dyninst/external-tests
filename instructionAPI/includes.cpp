#include "ArchSpecificFormatters.h"
#include "BinaryFunction.h"
#include "Dereference.h"
#include "Expression.h"
#include "Immediate.h"
#include "InstructionAST.h"
#include "InstructionCategories.h"
#include "InstructionDecoder.h"
#include "Instruction.h"
#include "Operand.h"
#include "Operation_impl.h"
#include "Register.h"
#include "Result.h"
#include "Ternary.h"
#include "Visitor.h"


int main() {
	namespace di = Dyninst::InstructionAPI;
	di::ArchSpecificFormatter *f;
	f->getInstructionString({});
}
