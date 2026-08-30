// @otlicense

#pragma once

#ifdef OPENTWINVIEWER_EXPORTS
//! @brief Dll export
#define OT_VIEWER_API_EXPORT __declspec(dllexport)
#else
//! @brief Dll import
#define OT_VIEWER_API_EXPORT __declspec(dllimport)
#endif // OPENTWINCORE_EXPORTS

#define OT_VIEWER_API_EXPORTONLY __declspec(dllexport)
