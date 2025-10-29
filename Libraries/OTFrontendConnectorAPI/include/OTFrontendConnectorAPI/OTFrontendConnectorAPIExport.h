// @otlicense

//! @file OTFrontendConnectorAPIExport.h
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

#ifdef OT_FRONTEND_CONNECTOR_API_EXPORTS
#define OT_FRONTEND_CONNECTOR_API_EXPORT __declspec(dllexport)
#else
#define OT_FRONTEND_CONNECTOR_API_EXPORT __declspec(dllimport)
#endif // OPENTWINCOMMUNICATION_EXPORTS
