// @otlicense

#pragma once

#ifdef RBE_OSG_WRAPPER_PROJ_BUILD
#define RBE_OSG_API_EXPORT __declspec(dllexport)
#else
#define RBE_OSG_API_EXPORT __declspec(dllimport)
#endif // RBE_OSG_WRAPPER_PROJ_BUILD
