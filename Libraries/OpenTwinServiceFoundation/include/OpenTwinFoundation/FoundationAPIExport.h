#pragma once

#ifdef OPENTWINFOUNDATION_EXPORTS
#define OT_SERVICEFOUNDATION_API_EXPORT __declspec(dllexport)
#else
#define OT_SERVICEFOUNDATION_API_EXPORT __declspec(dllimport)
#endif // OPENTWINFOUNDATION_EXPORTS