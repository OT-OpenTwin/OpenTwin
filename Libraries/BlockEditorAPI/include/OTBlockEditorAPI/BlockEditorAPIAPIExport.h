#pragma once

#ifdef BLOCKEDITORAPI_EXPORTS
#define BLOCKEDITORAPI_API_EXPORT __declspec(dllexport)
#else
#define BLOCKEDITORAPI_API_EXPORT __declspec(dllimport)
#endif // OPENTWINSYSTEM_EXPORTS
