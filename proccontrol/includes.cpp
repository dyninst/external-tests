#include "Event.h"
#include "EventType.h"
#include "Handler.h"
#include "Mailbox.h"
#include "PCErrors.h"
#include "PCProcess.h"
#include "PlatFeatures.h"
#include "ProcessSet.h"

int main() {
	namespace dp = Dyninst::ProcControlAPI;
	dp::Event d{dp::EventType::Unset};
}
