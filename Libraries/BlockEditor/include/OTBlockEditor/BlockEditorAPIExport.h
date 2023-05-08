//! @file BlockEditorAPIExport.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

#ifdef BLOCK_EDITOR_EXPORT
#define BLOCK_EDITOR_API_EXPORT __declspec(dllexport)
#else
#define BLOCK_EDITOR_API_EXPORT __declspec(dllimport)
#endif // BLOCK_EDITOR_EXPORT
