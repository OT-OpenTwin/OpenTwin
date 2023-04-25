#pragma once
#pragma warning(disable : 4251)

#include <string>

#include "TopoDS.hxx"

namespace GeometryChecker
{
	__declspec(dllexport) std::string CheckFace(TopoDS_Face aFace);
}

