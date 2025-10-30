// @otlicense

#pragma once

#ifdef OTBLOCKENTITIES_EXPORTS
#define OT_BLOCKENTITIES_API_EXPORT __declspec(dllexport)
#else
#define OT_BLOCKENTITIES_API_EXPORT __declspec(dllimport)
#endif // OTBLOCKENTITIES_EXPORTS

#define OT_BLOCKENTITIES_API_EXPORTONLY __declspec(dllexport)