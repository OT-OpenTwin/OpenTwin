#pragma once

#ifdef OPENTWINRANDOM_EXPORTS
#define OT_RANDOM_API_EXPORT __declspec(dllexport)
#else
#define OT_RANDOM_API_EXPORT __declspec(dllimport)
#endif // OPENTWINRANDOM_EXPORTS
