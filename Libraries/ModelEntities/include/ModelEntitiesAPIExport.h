// @otlicense

#pragma once

#ifdef OPENTWINMODELENTITIES_EXPORTS
//! @brief Dll export
#define OT_MODELENTITIES_API_EXPORT __declspec(dllexport)
#else
//! @brief Dll import
#define OT_MODELENTITIES_API_EXPORT __declspec(dllimport)
#endif // OPENTWINMODELENTITIES_EXPORTS

#define OT_MODELENTITIES_API_EXPORTONLY __declspec(dllexport)