#pragma once

#ifdef OPENTWINCOMMUNICATION_EXPORTS
#define OT_COMMUNICATION_API_EXPORT __declspec(dllexport)
#else
#define OT_COMMUNICATION_API_EXPORT __declspec(dllimport)
#endif // OPENTWINCOMMUNICATION_EXPORTS
