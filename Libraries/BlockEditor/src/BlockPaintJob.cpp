//! @file BlockPaintJob.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTBlockEditor/BlockPaintJob.h"
#include "OpenTwinCore/otAssert.h"

ot::BlockPaintJob::QueueResultFlags ot::BlockPaintJob::activateFromQueue(ot::AbstractQueue* _queue, ot::QueueData* _args) {
	ot::BlockPaintJobArg* args = dynamic_cast<ot::BlockPaintJobArg*>(_args);
	otAssert(args, "Invalid paint job data type");
	return runPaintJob(_queue, args);
}