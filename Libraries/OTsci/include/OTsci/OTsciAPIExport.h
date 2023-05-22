//! @file OTsciAPIExport.h
//! @author Alexander Kuester (alexk95)
//! @date November 2022
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

#ifdef OT_SCI_EXPORTS
#define OTSCI_API_EXPORT __declspec(dllexport)
#else
#define OTSCI_API_EXPORT __declspec(dllimport)
#endif
