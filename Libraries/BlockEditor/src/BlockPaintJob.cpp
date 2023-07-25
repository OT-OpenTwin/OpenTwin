//! @file BlockPaintJob.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTBlockEditor/BlockPaintJob.h"
#include "OpenTwinCore/Logger.h"

ot::BlockPaintJob::QueueResultFlags ot::BlockPaintJob::activateFromQueue(ot::AbstractQueue* _queue, ot::QueueData* _args) {
	ot::BlockPaintJobArg* args = dynamic_cast<ot::BlockPaintJobArg*>(_args);
	if (args == nullptr) {
		OT_LOG_EA("Invalid paint job data type");
		return BlockPaintJob::JobFailed;
	}
	else {
		return runPaintJob(_queue, args);
	}
}