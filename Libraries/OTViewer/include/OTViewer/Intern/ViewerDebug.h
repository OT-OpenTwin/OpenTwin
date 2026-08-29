// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/Debugging/DebugLogBase.h"

#define OT_VIEWER_MEM_DBG_ENABLED false
#if OT_VIEWER_MEM_DBG_ENABLED==true
#define OT_VIEWER_DBG(___text)                       OT_BASE_DEBUG_LOG("VIEWER", ___text)
#define OT_VIEWER_DBG_PTR(___ptr, ___text)           OT_BASE_DEBUG_PTR("VIEWER", ___ptr, ___text)
#else
#define OT_VIEWER_DBG(___text)
#define OT_VIEWER_DBG_PTR(___ptr, ___text)
#endif

#define OT_VIEWER_VIEWSEL_DBG_ENABLED false
#if OT_VIEWER_VIEWSEL_DBG_ENABLED==true
#define OT_VIEWER_VIEWSEL_DBG(___text)               OT_BASE_DEBUG_LOG("VIEWSEL][VIEWER", ___text)
#define OT_VIEWER_VIEWSEL_DBG_PTR(___ptr, ___text)   OT_BASE_DEBUG_PTR("VIEWSEL][VIEWER", ___ptr, ___text)
#else
#define OT_VIEWER_VIEWSEL_DBG(___text)
#define OT_VIEWER_VIEWSEL_DBG_PTR(___ptr, ___text)
#endif

#define OT_VIEWER_SCENENODE_DBG_ENABLED false
#if OT_VIEWER_SCENENODE_DBG_ENABLED==true
#define OT_VIEWER_SCENENODE_DBG(___text)             OT_BASE_DEBUG_LOG("SCENENODE][VIEWER", ___text)
#define OT_VIEWER_SCENENODE_DBG_PTR(___ptr, ___text) OT_BASE_DEBUG_PTR("SCENENODE][VIEWER", ___ptr, ___text)
#else
#define OT_VIEWER_SCENENODE_DBG(___text)
#define OT_VIEWER_SCENENODE_DBG_PTR(___ptr, ___text)
#endif

#define OT_VIEWER_VISUALISER_DBG_ENABLED false
#if OT_VIEWER_VISUALISER_DBG_ENABLED==true
#define OT_VIEWER_VISUALISER_DBG(___text)             OT_BASE_DEBUG_LOG("VISUALISER][VIEWER", ___text)
#define OT_VIEWER_VISUALISER_DBG_PTR(___ptr, ___text) OT_BASE_DEBUG_PTR("VISUALISER][VIEWER", ___ptr, ___text)
#else
#define OT_VIEWER_VISUALISER_DBG(___text)
#define OT_VIEWER_VISUALISER_DBG_PTR(___ptr, ___text)
#endif
