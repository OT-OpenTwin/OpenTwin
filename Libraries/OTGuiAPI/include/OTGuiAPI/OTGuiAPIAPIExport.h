// @otlicense

#pragma once

#ifdef OTGUIAPI_EXPORTS
#define OT_GUIAPI_API_EXPORT __declspec(dllexport)
#else
#define OT_GUIAPI_API_EXPORT __declspec(dllimport)
#endif // OTGUIAPI_EXPORTS

#define OT_GUIAPI_API_EXPORTONLY __declspec(dllexport)
