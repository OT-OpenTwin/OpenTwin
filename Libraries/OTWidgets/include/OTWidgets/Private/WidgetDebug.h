// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/Logging/LogDispatcher.h"

#define OT_WIDGETS_VIEW_DBG_ENABLED false
#if OT_WIDGETS_VIEW_DBG_ENABLED==true
#define OT_WIDGETS_VIEW_DBG(___text) OT_LOG_T(___text)
#define OT_WIDGETS_VIEW_DBG_PTR(___ptr, ___text) OT_LOG_MEM(___ptr, ___text)
#else
#define OT_WIDGETS_VIEW_DBG(___text)
#define OT_WIDGETS_VIEW_DBG_PTR(___ptr, ___text)
#endif