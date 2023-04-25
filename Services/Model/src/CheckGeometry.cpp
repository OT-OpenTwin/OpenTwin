#include "stdafx.h"

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
