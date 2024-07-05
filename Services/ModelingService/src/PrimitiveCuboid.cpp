#include "PrimitiveCuboid.h"

#include "EntityGeometry.h"

#include "OTServiceFoundation/UiComponent.h"

#include <rbeCore/RubberbandConfiguration.h>
#include <rbeCore/jsonMember.h>

#include <cassert>
#include <algorithm>

#include <BRepPrimAPI_MakeBox.hxx>

void PrimitiveCuboid::sendRubberbandData(void) 
{
	if (uiComponent == nullptr) {
		assert(0);
		return;
	}

	std::string cfgString;

	rbeCore::RubberbandConfiguration cfg;
	cfg.setToDefaultCuboid();
	cfgString = cfg.toJson();
	if (cfgString.empty()) { assert("Rubberband is empty"); }

	uiComponent->createRubberband("ModelingService.Cuboid", cfgString);
}

void PrimitiveCuboid::createFromRubberbandJson(const std::string& _json, std::vector<double> &_transform) 
{
	rapidjson::Document doc;
	doc.Parse(_json.c_str());

	if (doc.IsArray()) 
	{
		auto pointData = doc.GetArray();

		if (pointData.Size() == 4 || pointData.Size() == 8) 
		{
			gp_Pnt p1;
			gp_Pnt p2;
			gp_Pnt p3;
			gp_Pnt p4;			

			double w1;
			double w5;

			bool p1Set{ false };
			bool p2Set{ false };
			bool p3Set{ false };
			bool p4Set{ false };
			bool p5Set{ false };
			bool p6Set{ false };

			for (rapidjson::SizeType i{ 0 }; i < pointData.Size(); i++) {
				if (!pointData[i].IsObject()) {
					if (uiComponent) { uiComponent->displayWarningPrompt("Cuboid json document is broken: Entry is not an object"); }
					else { assert(0); }
					return;
				}
				auto ptObj = pointData[i].GetObject();
				if (!ptObj.HasMember(RBE_JSON_Point_ID)) {
					if (uiComponent) { uiComponent->displayWarningPrompt("Cuboid json document is broken: Member \"" RBE_JSON_Point_ID "\" is missing"); }
					else { assert(0); }
					return;
				}
				else if (!ptObj.HasMember(RBE_JSON_Point_U)) {
					if (uiComponent) { uiComponent->displayWarningPrompt("Cuboid json document is broken: Member \"" RBE_JSON_Point_U "\" is missing"); }
					else { assert(0); }
					return;
				}
				else if (!ptObj.HasMember(RBE_JSON_Point_V)) {
					if (uiComponent) { uiComponent->displayWarningPrompt("Cuboid json document is broken: Member \"" RBE_JSON_Point_V "\" is missing"); }
					else { assert(0); }
					return;
				}
				else if (!ptObj.HasMember(RBE_JSON_Point_W)) {
					if (uiComponent) { uiComponent->displayWarningPrompt("Cuboid json document is broken: Member \"" RBE_JSON_Point_W "\" is missing"); }
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
					w5 = pZ;
					p5Set = true;
				}
			}

			if (!p1Set || !p2Set || !p3Set || !p4Set || !p5Set) {
				if (uiComponent) {
					uiComponent->displayWarningPrompt("Modeling: Not all required points were provided for create cylinder from json");
				}
				else { assert(0); }
				return;
			}

			double xmin = std::min(p1.X(), std::min(p2.X(), std::min(p3.X(), p4.X())));
			double xmax = std::max(p1.X(), std::max(p2.X(), std::max(p3.X(), p4.X())));

			double ymin = std::min(p1.Y(), std::min(p2.Y(), std::min(p3.Y(), p4.Y())));
			double ymax = std::max(p1.Y(), std::max(p2.Y(), std::max(p3.Y(), p4.Y())));

			double zmin = std::min(w1, w5);
			double zmax = std::max(w1, w5);

			TopoDS_Shape box = BRepPrimAPI_MakeBox(gp_Pnt(xmin, ymin, zmin), gp_Pnt(xmax, ymax, zmax));

			std::list<std::pair<std::string, std::string>> shapeParameters;

			shapeParameters.push_back(std::pair<std::string, std::string>("Xmin", to_string(xmin)));
			shapeParameters.push_back(std::pair<std::string, std::string>("Xmax", to_string(xmax)));
			shapeParameters.push_back(std::pair<std::string, std::string>("Ymin", to_string(ymin)));
			shapeParameters.push_back(std::pair<std::string, std::string>("Ymax", to_string(ymax)));
			shapeParameters.push_back(std::pair<std::string, std::string>("Zmin", to_string(zmin)));
			shapeParameters.push_back(std::pair<std::string, std::string>("Zmax", to_string(zmax)));

			std::list<std::string> faceNames = { "f1", "f2", "f3", "f4", "f5", "f6" };

			storeShapeInModel(box, _transform, "Geometry/Cuboid", "Cuboid", shapeParameters, faceNames);
		}
		else 
		{
			if (uiComponent) { uiComponent->displayWarningPrompt("Invalid number of points received for create cuboid from rubberband"); }
			else { assert(0); }
		}
	}
	else 
	{
		assert(0); // Document is not an array
	}
}

void PrimitiveCuboid::update(EntityGeometry *geomEntity, TopoDS_Shape &shape)
{
	EntityPropertiesDouble *xminProperty = dynamic_cast<EntityPropertiesDouble*>(geomEntity->getProperties().getProperty("#Xmin"));
	EntityPropertiesDouble *xmaxProperty = dynamic_cast<EntityPropertiesDouble*>(geomEntity->getProperties().getProperty("#Xmax"));
	EntityPropertiesDouble *yminProperty = dynamic_cast<EntityPropertiesDouble*>(geomEntity->getProperties().getProperty("#Ymin"));
	EntityPropertiesDouble *ymaxProperty = dynamic_cast<EntityPropertiesDouble*>(geomEntity->getProperties().getProperty("#Ymax"));
	EntityPropertiesDouble *zminProperty = dynamic_cast<EntityPropertiesDouble*>(geomEntity->getProperties().getProperty("#Zmin"));
	EntityPropertiesDouble *zmaxProperty = dynamic_cast<EntityPropertiesDouble*>(geomEntity->getProperties().getProperty("#Zmax"));

	if (xminProperty == nullptr || yminProperty == nullptr || zminProperty == nullptr || xmaxProperty == nullptr || ymaxProperty == nullptr || zmaxProperty == nullptr)
	{
		assert(0); // Some properties are missing
		return;
	}

	double xminE = xminProperty->getValue();
	double xmaxE = xmaxProperty->getValue();
	double yminE = yminProperty->getValue();
	double ymaxE = ymaxProperty->getValue();
	double zminE = zminProperty->getValue();
	double zmaxE = zmaxProperty->getValue();

	double xmin = std::min(xminE, xmaxE);
	double xmax = std::max(xminE, xmaxE);
	double ymin = std::min(yminE, ymaxE);
	double ymax = std::max(yminE, ymaxE);
	double zmin = std::min(zminE, zmaxE);
	double zmax = std::max(zminE, zmaxE);

	try
	{
		shape = BRepPrimAPI_MakeBox(gp_Pnt(xmin, ymin, zmin), gp_Pnt(xmax, ymax, zmax));
		
	}
	catch (Standard_Failure)
	{
		assert(0); // Invalid parameters, shape creation failed.
	}

	std::list<std::string> faceNames = { "f1", "f2", "f3", "f4", "f5", "f6" };

	applyFaceNames(geomEntity, shape, faceNames);
}
