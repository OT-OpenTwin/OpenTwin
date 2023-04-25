#pragma once

#ifdef UIPLUGINAPI_API_EXPORT
#define UIPLUGINAPI_EXPORT __declspec(dllexport)
#else
#define UIPLUGINAPI_EXPORT __declspec(dllimport)
#endif // UIPLUGINAPI_API_EXPORT