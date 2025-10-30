// @otlicense

#pragma once

#ifdef OT_FRONTEND_CONNECTOR_API_EXPORTS
#define OT_FRONTEND_CONNECTOR_API_EXPORT __declspec(dllexport)
#else
#define OT_FRONTEND_CONNECTOR_API_EXPORT __declspec(dllimport)
#endif // OPENTWINCOMMUNICATION_EXPORTS
