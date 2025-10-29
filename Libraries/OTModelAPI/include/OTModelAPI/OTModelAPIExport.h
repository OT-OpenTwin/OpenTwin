// @otlicense

//! @file OTModelAPIExport.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

#ifdef OTMODELAPI_EXPORTS
#define OT_MODELAPI_API_EXPORT __declspec(dllexport)
#else
#define OT_MODELAPI_API_EXPORT __declspec(dllimport)
#endif // OPENTWINSYSTEM_EXPORTS

#define OT_MODELAPI_API_EXPORTONLY __declspec(dllexport)
