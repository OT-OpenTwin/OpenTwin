//! @file SystemAPIExport.h
//! @author Alexander Kuester (alexk95)
//! @date April 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

#ifdef OPENTWINSYSTEM_EXPORTS
#define OT_SYS_API_EXPORT __declspec(dllexport)
#else
#define OT_SYS_API_EXPORT __declspec(dllimport)
#endif // OPENTWINSYSTEM_EXPORTS
