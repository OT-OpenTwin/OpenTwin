//! @file OTQtWrapperAPIExport.h
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

#ifdef OT_QTWRAPPER_EXPORTS
#define OT_QTWRAPPER_API_EXPORT __declspec(dllexport)
#else
#define OT_QTWRAPPER_API_EXPORT __declspec(dllimport)
#endif // OPENTWINSYSTEM_EXPORTS
