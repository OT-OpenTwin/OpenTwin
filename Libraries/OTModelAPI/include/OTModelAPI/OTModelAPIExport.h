// @otlicense

#pragma once

#ifdef OTMODELAPI_EXPORTS
#define OT_MODELAPI_API_EXPORT __declspec(dllexport)
#else
#define OT_MODELAPI_API_EXPORT __declspec(dllimport)
#endif // OPENTWINSYSTEM_EXPORTS

#define OT_MODELAPI_API_EXPORTONLY __declspec(dllexport)
