#pragma once

#ifdef BLOCK_EDITOR_EXPORT
#define BLOCK_EDITOR_API_EXPORT __declspec(dllexport)
#else
#define BLOCK_EDITOR_API_EXPORT __declspec(dllimport)
#endif // BLOCK_EDITOR_EXPORT
