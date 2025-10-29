// @otlicense

#pragma once

// This is used to initialize the group
//! @addtogroup OTGui

#ifdef OTGUI_EXPORTS
#define OT_GUI_API_EXPORT __declspec(dllexport)
#else
#define OT_GUI_API_EXPORT __declspec(dllimport)
#endif // OPENTWINSYSTEM_EXPORTS

#define OT_GUI_API_EXPORTONLY __declspec(dllexport)
