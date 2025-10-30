// @otlicense
// File: CheckGeometry.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

//#include "stdafx.h"

#include "TopoDS.hxx"
#include "Geom_Surface.hxx"
#include "Geom_ToroidalSurface.hxx"
#include "BRepLProp_SLProps.hxx"
#include "Precision.hxx"

#include "CheckGeometry.h"

std::string GeometryChecker::CheckFace(TopoDS_Face aFace)
{
	BRepAdaptor_Surface surface(aFace, Standard_False);
	std::string error;

	GeomAbs_SurfaceType surfaceType = surface.GetType();

	switch (surfaceType)
	{
	case GeomAbs_Torus:
		{
			Handle(Geom_ToroidalSurface) torus = Handle(Geom_ToroidalSurface)::DownCast(surface.Surface().Surface());

			// Check for degeneration of torus (minor radius >= majow radius)
			if (torus->MajorRadius() - torus->MinorRadius() < Precision::Confusion())
				error = "degenerated torodial face (minor radius (" + std::to_string(torus->MinorRadius()) + ") >= major radius (" + std::to_string(torus->MajorRadius()) + ") )";
		}
		break;
	default:
		// Unknown error
		//assert(0);
		break;
	}
	
	return error;
}
