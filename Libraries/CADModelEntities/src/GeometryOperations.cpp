// @otlicense
// File: GeometryOperations.cpp
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


#include "BRepMesh_IncrementalMesh.hxx"
#include "TopExp_Explorer.hxx"
#include "BRepLProp_SLProps.hxx"
#include "BRepAlgoAPI_BuilderAlgo.hxx"
#include "BRep_Builder.hxx"
#include "BRepAlgoAPI_Fuse.hxx"
#include "BRepPrimAPI_MakeSphere.hxx"
#include "gp_Pnt.hxx"
#include "gp_Cone.hxx"
#include "gp_Sphere.hxx"
#include "NCollection_Sequence.hxx"
#include "TColgp_SequenceOfPnt.hxx"
#include "BRepTools.hxx"

#include "TopTools_IndexedMapOfShape.hxx"
#include "TopExp.hxx"
#include "TopoDS_Face.hxx"
#include "TopoDS_TFace.hxx"
#include "TopoDS_Compound.hxx"

#include "EntityGeometry.h"
#include "GeometryOperations.h"
#include "CheckGeometry.h"
#include "TemplateDefaultManager.h"

bool GeometryOperations::facetEntity(TopoDS_Shape &shape, EntityBrep *brep, double deflection, std::vector<Geometry::Node> &nodes, std::list<Geometry::Triangle> &triangles, std::list<Geometry::Edge> &edges, std::map<ot::UID, std::string>& faceNames, std::string &errors)
{
	bool success = true;

	size_t pointOffset = nodes.size();

	if (fabs(deflection) < Precision::Confusion())
	{
		// In case the shape is empty, we will have trouble using a deflection of 0.0 for the mesh generation
		deflection = 1.0;
	}

	Standard_Real aDeflection = deflection;
	BRepMesh_IncrementalMesh(shape, aDeflection);
	Standard_Integer aIndex = 1, nbNodes = 0;
	TColgp_SequenceOfPnt aPoints, aPoints1;

	ot::UID faceId = 0;

	// Set the starting indices if the triangle list is not empty
	if (!triangles.empty())
	{
		ot::UID maxFaceID = 0;

		for (auto triangle : triangles)
		{
			maxFaceID = std::max(maxFaceID, triangle.getFaceId());
		}

		maxFaceID++;
	}

	for (TopExp_Explorer aExpFace(shape, TopAbs_FACE); aExpFace.More(); aExpFace.Next())
	{
		TopoDS_Face aFace = TopoDS::Face(aExpFace.Current());
		TopAbs_Orientation faceOrientation = aFace.Orientation();

		double umin = 0.0, vmin = 0.0, umax = 0.0, vmax = 0.0;
		BRepTools::UVBounds(aFace, umin, umax, vmin, vmax);

		double deltaU = umax - umin;
		double deltaV = vmax - vmin;

		if (deltaU == 0.0) deltaU = 1.0;
		if (deltaV == 0.0) deltaV = 1.0;

		BRepAdaptor_Surface surface(aFace, Standard_False);
		BRepLProp_SLProps surfaceProps(surface, 1, 1e-5);

		TopLoc_Location aLocation;
		Handle(Poly_Triangulation) aTr = BRep_Tool::Triangulation(aFace, aLocation);

		TopExp_Explorer edgeExplorer(aFace, TopAbs_EDGE);
		while (edgeExplorer.More())
		{
			const TopoDS_Edge& edge(TopoDS::Edge(edgeExplorer.Current()));
			Handle(Poly_PolygonOnTriangulation) edgeMesh;
			edgeMesh = BRep_Tool::PolygonOnTriangulation(edge, aTr, aLocation);

			gp_Trsf myTransf;
			if (!aLocation.IsIdentity()) {
				myTransf = aLocation.Transformation();
			}

			if (!edgeMesh.IsNull())
			{
				// getting size and create the array
				Standard_Integer nEdgeNodes = edgeMesh->NbNodes();

				const TColStd_Array1OfInteger& indices = edgeMesh->Nodes();

				gp_Pnt V;
				int pos = 0;
				// go through the index array
				Geometry::Edge edge;
				edge.setFaceId(faceId);
				edge.setNpoints(nEdgeNodes);
				int index = 0;

				for (Standard_Integer i = indices.Lower(); i <= indices.Upper(); i++) {
					V = aTr->Node(indices(i));
					V.Transform(myTransf);

					edge.setPoint(index, V.X(), V.Y(), V.Z());
					index++;
				}

				edges.push_back(edge);
			}

			edgeExplorer.Next();
		}

		bool normalOk = true;

		if (!aTr.IsNull())
		{
			const Poly_Array1OfTriangle& facetriangles = aTr->Triangles();
			
			for (Standard_Integer i = 1; i <= aTr->NbNodes(); i++)
			{
				gp_Pnt   point = aTr->Node(i).Transformed(aLocation);
				gp_Pnt2d uv = aTr->UVNode(i);

				gp_Vec   normal;
				surfaceProps.SetParameters(uv.X(), uv.Y());
				if (surfaceProps.IsNormalDefined())
				{
					if (surface.GetType() == GeomAbs_Sphere && fabs(uv.Y() - M_PI * 0.5) < 0.0001)
					{
						// We are in the tip of a sphere (the normal is not unique here)
						normal = gp_Vec(0.0, 0.0, 1.0);
					}
					else
					{
						normal = surfaceProps.Normal();// .Transformed(aLocation);
					}

					if (faceOrientation != TopAbs_Orientation::TopAbs_FORWARD) normal *= -1.0;
				}
				else if (surface.GetType() == GeomAbs_Cone)
				{
					// We are in the tip of the cone

					gp_Cone cone = surface.Cone();
					gp_Dir  axis = cone.Axis().Direction();

					gp_Trsf myTransf;
					if (!aLocation.IsIdentity()) {
						myTransf = aLocation.Transformation();
					}

					normal = gp_Vec(axis.X(), axis.Y(), axis.Z());
					normal.Transform(myTransf);
				}
				else
				{
					normalOk = false;
					normal = gp_Vec(0.0, 0.0, 0.0);
				}

				Geometry::Node node;
				node.setCoords(point.X(), point.Y(), point.Z());
				node.setNormals(normal.X(), normal.Y(), normal.Z());

				double upar = (uv.X() - umin) / deltaU;
				double vpar = (uv.Y() - vmin) / deltaV;
				node.setUVpar(upar, vpar);

				nodes.push_back(node);
			}

			Standard_Integer nnn = aTr->NbTriangles();
			Standard_Integer nt, n1, n2, n3;

			for (nt = 1; nt < nnn + 1; nt++)
			{
				if (faceOrientation == TopAbs_Orientation::TopAbs_FORWARD)
				{
					facetriangles(nt).Get(n1, n2, n3);
				}
				else
				{
					facetriangles(nt).Get(n3, n2, n1);
				}

				triangles.push_back(Geometry::Triangle(n1-1 + pointOffset, n2-1 + pointOffset, n3-1 + pointOffset, faceId));
			}

			pointOffset += aTr->NbNodes();
		}

		if (!normalOk)
		{
			std::string error = GeometryChecker::CheckFace(aFace);

			std::string surfaceTypes[] = { "plane", "cylinder", "cone", "sphere", "torus", "bezier surface", "BSpline surface", "surface of revolution", 
										   "surface of extrusion", "offset surface", "other surface" };

			std::string surfaceType;
			if (surface.GetType() < 0 || surface.GetType() > 10)
			{
				surfaceType = "unknown";
			}
			else
			{
				surfaceType = surfaceTypes[surface.GetType()];
			}

			if (error == "") error = "unknown (surface type: " + surfaceType + ")";

			errors += "ERROR: facetter unable to compute vertex normal, reason: " + error + "\n";
			success = false;
		}

		if (brep != nullptr)
		{
			faceNames[faceId] = brep->getFaceName(aFace);
		}

		faceId++;
	}

	std::cout << "Shape facetted, triangles generated: " << triangles.size() << std::endl;
	
	return success;
}

BoundingBox GeometryOperations::getBoundingBox(std::list<EntityGeometry *> &geometryEntities)
{
	BoundingBox boundingBox;

	for (auto entity : geometryEntities)
	{
		double xmin{ 0.0 }, xmax{ 0.0 }, ymin{ 0.0 }, ymax{ 0.0 }, zmin{ 0.0 }, zmax{ 0.0 };
		if (entity->getEntityBox(xmin, xmax, ymin, ymax, zmin, zmax))
		{
			boundingBox.extend(xmin, xmax, ymin, ymax, zmin, zmax);
		}
	}

	return boundingBox;
}

bool GeometryOperations::checkPointInTriangle(double x, double y, double z,
											  double nodeCoord1[], double nodeCoord2[], double nodeCoord3[],
											  double tolerance)
{
	double pointCoord[3] = { x, y, z };

	double dist = calculateDistancePointToPlane(pointCoord, nodeCoord1, nodeCoord2, nodeCoord3);
	if (fabs(dist) > tolerance) return false;

	if (   SameSide(pointCoord, nodeCoord1, nodeCoord2, nodeCoord3)
		&& SameSide(pointCoord, nodeCoord2, nodeCoord1, nodeCoord3)
		&& SameSide(pointCoord, nodeCoord3, nodeCoord1, nodeCoord2))
	{
		return true;
	}

	return false;
}

double GeometryOperations::calculateDistancePointToPlane(double p[], double v1[], double v2[], double v3[])
{
	// Calculate plane normal
	double v21[3] = { v2[0] - v1[0], v2[1] - v1[1], v2[2] - v1[2] };
	double v31[3] = { v3[0] - v1[0], v3[1] - v1[1], v3[2] - v1[2] };

	double norm[3];
	calculateCrossProduct(v21, v31, norm);

	// Normalize normal vector
	double normAbs = sqrt(norm[0]*norm[0] + norm[1] * norm[1]+ norm[2] * norm[2]);
	if (normAbs > 0.0)
	{
		norm[0] /= normAbs;
		norm[1] /= normAbs;
		norm[2] /= normAbs;
	}

	// Calculate distance of point to plane
	double vp1[3] = { p[0] - v1[0], p[1] - v1[1], p[2] - v1[2] };

	double distance = calculateDotProduct(vp1, norm);

	return distance;
}

bool GeometryOperations::SameSide(double p1[], double p2[], double a[], double b[])
{
	double bma[3]  = {  b[0] - a[0],  b[1] - a[1],  b[2] - a[2] };
	double p1ma[3] = { p1[0] - a[0], p1[1] - a[1], p1[2] - a[2] };
	double p2ma[3] = { p2[0] - a[0], p2[1] - a[1], p2[2] - a[2] };

	double cp1[3], cp2[3];

	calculateCrossProduct(bma, p1ma, cp1);
	calculateCrossProduct(bma, p2ma, cp2);

	double dprod = calculateDotProduct(cp1, cp2);

	return (dprod >= 0.0);
}

void GeometryOperations::calculateCrossProduct(double vector1[3], double vector2[3], double normal[3])
{
	normal[0] = vector1[1] * vector2[2] - vector1[2] * vector2[1];
	normal[1] = vector1[2] * vector2[0] - vector1[0] * vector2[2];
	normal[2] = vector1[0] * vector2[1] - vector1[1] * vector2[0];
}

double GeometryOperations::calculateDotProduct(double vector1[3], double vector2[3])
{
	return (vector1[0] * vector2[0] + vector1[1] * vector2[1] + vector1[2] * vector2[2]);
}

double GeometryOperations::getMaximumFaceCurvature(TopoDS_Face &aFace)
{
	BRepAdaptor_Surface surface(aFace, Standard_False);
	BRepLProp_SLProps surfaceProps(surface, 2, 1e-5);

	double uParam = 0.5 * surface.FirstUParameter() + 0.5 * surface.LastUParameter();
	double vParam = 0.5 * surface.FirstVParameter() + 0.5 * surface.LastVParameter();

	surfaceProps.SetParameters(uParam, vParam);

	double maxCurvature = 0.0;

	if (surfaceProps.IsCurvatureDefined())
	{
		maxCurvature = std::max(fabs(surfaceProps.MinCurvature()), fabs(surfaceProps.MaxCurvature()));
	}

	return maxCurvature;
}

std::vector<std::pair<std::pair<double, double>, int>> GeometryOperations::getCurvatureRadiusHistogram(std::list<double> &faceCurvatureRadius, int nbins)
{
	std::vector<std::pair<std::pair<double, double>, int>> histogram;

	// Determine min and max values

	double minRadius = faceCurvatureRadius.front();
	double maxRadius = faceCurvatureRadius.front();

	for (auto curvature : faceCurvatureRadius)
	{
		minRadius = std::min(minRadius, curvature);
		maxRadius = std::max(maxRadius, curvature);
	}

	// Now determine subdivisions

	if (fabs(minRadius - maxRadius) < 1e-6)
	{
		histogram.push_back(std::pair<std::pair<double, double>, int>(std::pair<double, double>(minRadius, maxRadius), (int)faceCurvatureRadius.size()));
	}
	else
	{
		double step = (maxRadius - minRadius) / nbins;

		for (int i = 0; i < nbins; i++)
		{
			histogram.push_back(std::pair<std::pair<double, double>, int>(std::pair<double, double>(minRadius + i*step, minRadius + (i+1) * step), 0));
		}

		for (auto curvature : faceCurvatureRadius)
		{
			int index = std::min((int) ((curvature - minRadius) / step), nbins-1);

			std::pair<std::pair<double, double>, int> item = histogram[index];
			item.second++;
			histogram[index] = item;
		}
	}

	return histogram;
}

