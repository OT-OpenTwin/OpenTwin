#pragma once

#ifdef OPENTWINFOUNDATION_EXPORTS
#define OT_SERVICEFOUNDATION_API_EXPORT __declspec(dllexport)
#define OT_SERVICEFOUNDATION_API_EXPORTONLY __declspec(dllexport)
#else
#define OT_SERVICEFOUNDATION_API_EXPORT __declspec(dllimport)
#define OT_SERVICEFOUNDATION_API_EXPORTONLY __declspec(dllexport)
#endif // OPENTWINFOUNDATION_EXPORTS