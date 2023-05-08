#pragma once

#ifdef OT_WIDGETS_EXPORTS
#define OT_WIDGETS_API_EXPORT __declspec(dllexport)
#define OT_WIDGETS_API_EXPORTONLY __declspec(dllexport)
#else
#define OT_WIDGETS_API_EXPORT __declspec(dllimport)
#define OT_WIDGETS_API_EXPORTONLY
#endif