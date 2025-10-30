// @otlicense
// File: PrimitiveCylinder.cpp
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

#include "PrimitiveCylinder.h"

#include "EntityGeometry.h"

#include "OTServiceFoundation/UiComponent.h"

#include <rbeCore/RubberbandConfiguration.h>
#include <rbeCore/jsonMember.h>

#include <cassert>
#include <algorithm>

#include <BRepPrimAPI_MakeCylinder.hxx>

void PrimitiveCylinder::sendRubberbandData(void) 
{
	if (uiComponent == nullptr) {
		assert(0);
		return;
	}

	std::string cfgString;

	rbeCore::RubberbandConfiguration cfg;
	cfg.setToDefaultCylinder();
	cfgString = cfg.toJson();
	if (cfgString.empty()) { assert("Rubberband is empty"); }

	uiComponent->createRubberband("ModelingService.Cylinder", cfgString);
}

void PrimitiveCylinder::createFromRubberbandJson(const std::string& _json, std::vector<double> &_transform) 
{
	rapidjson::Document doc;
	doc.Parse(_json.c_str());

	if (doc.IsArray()) {
		auto pointData = doc.GetArray();

		if (pointData.Size() == 9 || pointData.Size() == 14) {
			gp_Pnt p1;
			gp_Pnt p2;
			gp_Pnt p3;
			gp_Pnt p4;
			gp_Pnt p5;

			double w1;
			double w6;

			bool p1Set{ false };
			bool p2Set{ false };
			bool p3Set{ false };
			bool p4Set{ false };
			bool p5Set{ false };
			bool p6Set{ false };

			for (rapidjson::SizeType i{ 0 }; i < pointData.Size(); i++) {
				if (!pointData[i].IsObject()) {
					if (uiComponent) { uiComponent->displayWarningPrompt("Cylinder json document is broken: Entry is not an object"); }
					else { assert(0); }
					return;
				}
				auto ptObj = pointData[i].GetObject();
				if (!ptObj.HasMember(RBE_JSON_Point_ID)) {
					if (uiComponent) { uiComponent->displayWarningPrompt("Cylinder json document is broken: Member \"" RBE_JSON_Point_ID "\" is missing"); }
					else { assert(0); }
					return;
				}
				else if (!ptObj.HasMember(RBE_JSON_Point_U)) {
					if (uiComponent) { uiComponent->displayWarningPrompt("Cylinder json document is broken: Member \"" RBE_JSON_Point_U "\" is missing"); }
					else { assert(0); }
					return;
				}
				else if (!ptObj.HasMember(RBE_JSON_Point_V)) {
					if (uiComponent) { uiComponent->displayWarningPrompt("Cylinder json document is broken: Member \"" RBE_JSON_Point_V "\" is missing"); }
					else { assert(0); }
					return;
				}
				else if (!ptObj.HasMember(RBE_JSON_Point_W)) {
					if (uiComponent) { uiComponent->displayWarningPrompt("Cylinder json document is broken: Member \"" RBE_JSON_Point_W "\" is missing"); }
					else { assert(0); }
					return;
				}

				int pId = ptObj[RBE_JSON_Point_ID].GetInt();
				double pX = ptObj[RBE_JSON_Point_U].GetDouble();
				double pY = ptObj[RBE_JSON_Point_V].GetDouble();
				double pZ = ptObj[RBE_JSON_Point_W].GetDouble();

				if (pId == 1) {
					p1.SetX(pX);
					p1.SetY(pY);
					p1.SetZ(pZ);
					w1 = pZ;
					p1Set = true;
				}
				else if (pId == 2) {
					p2.SetX(pX);
					p2.SetY(pY);
					p2.SetZ(pZ);
					p2Set = true;
				}
				else if (pId == 3) {
					p3.SetX(pX);
					p3.SetY(pY);
					p3.SetZ(pZ);
					p3Set = true;
				}
				else if (pId == 4) {
					p4.SetX(pX);
					p4.SetY(pY);
					p4.SetZ(pZ);
					p4Set = true;
				}
				else if (pId == 5) {
					p5.SetX(pX);
					p5.SetY(pY);
					p5.SetZ(pZ);
					p5Set = true;
				}
				else if (pId == 6) {
					w6 = pZ;
					p6Set = true;
				}
			}

			if (!p1Set || !p2Set || !p3Set || !p4Set || !p5Set || !p6Set) {
				if (uiComponent) {
					uiComponent->displayWarningPrompt("Modeling: Not all required points were provided for create cylinder from json");
				}
				else { assert(0); }
				return;
			}

			double xcenter = p1.X();
			double ycenter = p1.Y();
			double zmin = w1;
			double zmax = w6;

			double radiusX = p1.X() - p2.X();
			double radiusY = p1.Y() - p2.Y();
			double radiusZ = p1.Z() - p2.Z();
			double radius = sqrt(radiusX*radiusX + radiusY*radiusY + radiusZ*radiusZ);

			gp_Pnt centerPoint = gp_Pnt(xcenter, ycenter, zmin);
			gp_Dir direction;
			TopoDS_Shape body;

			std::list<std::string> faceNames;
			
			if (fabs(zmax - zmin) > Precision::Confusion() && radius > Precision::Confusion())
			{
				faceNames = { "f1", "f2", "f3" };

				if (zmax > zmin) {		// if the z component is in positive z-axis direction
					direction = gp_Dir(0, 0, 1);
					body = BRepPrimAPI_MakeCylinder(gp_Ax2(centerPoint, direction), radius, zmax);
				}
				else {					// if the z component is in negative z-axis direction
					direction = gp_Dir(0, 0, -1);
					zmax = fabs(zmax);
					body = BRepPrimAPI_MakeCylinder(gp_Ax2(centerPoint, direction), radius, zmax);
					zmax *= (-1);
				}
			}

			std::list<std::pair<std::string, std::string>> shapeParameters;
			shapeParameters.push_back(std::pair<std::string, std::string>("XCenter", to_string(xcenter)));
			shapeParameters.push_back(std::pair<std::string, std::string>("YCenter", to_string(ycenter)));
			shapeParameters.push_back(std::pair<std::string, std::string>("ZMin",	 to_string(zmin)));
			shapeParameters.push_back(std::pair<std::string, std::string>("ZMax",	 to_string(zmax)));
			shapeParameters.push_back(std::pair<std::string, std::string>("Radius",  to_string(radius)));

			storeShapeInModel(body, _transform, "Geometry/Cylinder", "Cylinder", shapeParameters, faceNames);
		}
		else {
			if (uiComponent) { uiComponent->displayWarningPrompt("Invalid number of points received for create cylinder from rubberband"); }
			else { assert(0); }
		}
	}
	else {
		assert(0); // Document is not an array
	}
}

void PrimitiveCylinder::update(EntityGeometry *geomEntity, TopoDS_Shape &shape)
{
	EntityPropertiesDouble *xCenterProperty	= dynamic_cast<EntityPropertiesDouble *>(geomEntity->getProperties().getProperty("#XCenter"));
	EntityPropertiesDouble *yCenterProperty	= dynamic_cast<EntityPropertiesDouble *>(geomEntity->getProperties().getProperty("#YCenter"));
	EntityPropertiesDouble *zMinProperty    = dynamic_cast<EntityPropertiesDouble *>(geomEntity->getProperties().getProperty("#ZMin"));
	EntityPropertiesDouble *zMaxProperty    = dynamic_cast<EntityPropertiesDouble *>(geomEntity->getProperties().getProperty("#ZMax"));
	EntityPropertiesDouble *radiusProperty  = dynamic_cast<EntityPropertiesDouble *>(geomEntity->getProperties().getProperty("#Radius"));

	if (xCenterProperty == nullptr || yCenterProperty == nullptr || zMinProperty == nullptr || zMaxProperty == nullptr || radiusProperty == nullptr) {
		assert(0);
		return;
	}

	double xCenter = xCenterProperty->getValue();
	double yCenter = yCenterProperty->getValue();
	double zMin    = zMinProperty->getValue();
	double zMax    = zMaxProperty->getValue();
	double radius  = radiusProperty->getValue();

	gp_Pnt centerPoint = gp_Pnt(xCenter, yCenter, zMin);
	gp_Dir direction;

	std::list<std::string> faceNames;

	if (fabs(zMax - zMin) > Precision::Confusion() && radius > Precision::Confusion())
	{
		try {
			if (zMax > zMin) {		// if the z component is in positive z-axis direction
				direction = gp_Dir(0, 0, 1);
				shape = BRepPrimAPI_MakeCylinder(gp_Ax2(centerPoint, direction), radius, zMax - zMin);
			}
			else {					// if the z component is in negative z-axis direction
				direction = gp_Dir(0, 0, -1);
				shape = BRepPrimAPI_MakeCylinder(gp_Ax2(centerPoint, direction), radius, zMin - zMax);
			}

			faceNames = { "f1", "f2", "f3" };
		}
		catch (Standard_Failure) {
			assert(0);
		}
	}
	else
	{
		TopoDS_Shape empty;
		shape = empty;
	}

	applyFaceNames(geomEntity, shape, faceNames);
}
