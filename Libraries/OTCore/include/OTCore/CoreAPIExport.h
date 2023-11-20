#pragma once

#ifdef OPENTWINCORE_EXPORTS
//! @brief Dll export
#define OT_CORE_API_EXPORT __declspec(dllexport)
#else
//! @brief Dll import
#define OT_CORE_API_EXPORT __declspec(dllimport)
#endif // OPENTWINCORE_EXPORTS

#define OT_CORE_API_EXPORTONLY __declspec(dllexport)
