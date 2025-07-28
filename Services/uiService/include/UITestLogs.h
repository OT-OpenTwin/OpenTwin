#pragma once

// OpenTwin header
#include "OTCore/Logger.h"

#define OT_SELECTION_TEST_LOGS_ENABLED false

#if OT_SELECTION_TEST_LOGS_ENABLED==true
#define OT_SLECTION_TEST_LOG(___text) OT_LOG_T(___text)
#else
#define OT_SLECTION_TEST_LOG(___text)
#endif