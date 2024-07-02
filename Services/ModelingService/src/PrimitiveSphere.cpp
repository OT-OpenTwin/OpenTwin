#include "PrimitiveSphere.h"

#include "EntityGeometry.h"

#include "OTServiceFoundation/UiComponent.h"

#include <rbeCore/RubberbandConfiguration.h>
#include <rbeCore/jsonMember.h>

#include <cassert>
#include <algorithm>

#include <BRepPrimAPI_MakeSphere.hxx>

void PrimitiveSphere::sendRubberbandData(void) 
{
	if (uiComponent == nullptr) {
		assert(0);
		return;
	}

	std::string cfgString;

	rbeCore::RubberbandConfiguration cfg;
	cfg.setToDefaultSphere();
	cfgString = cfg.toJson();
	if (cfgString.empty()) {
		assert("Rubberband is empty");
	}

	uiComponent->createRubberband("ModelingService.Sphere", cfgString);
}

void PrimitiveSphere::createFromRubberbandJson(const std::string& _json, std::vector<double> &_transform) 
{
	rapidjson::Document doc;
	doc.Parse(_json.c_str());

	if (doc.IsArray()) {
		auto pointData = doc.GetArray();

		if (pointData.Size() == 7) {
			gp_Pnt p1;
			gp_Pnt p2;
			gp_Pnt p3;
			gp_Pnt p4;
			gp_Pnt p5;
			gp_Pnt p6;
			gp_Pnt p7;

			bool p1Set = false;
			bool p2Set = false;
			bool p3Set = false;
			bool p4Set = false;
			bool p5Set = false;
			bool p6Set = false;
			bool p7Set = false;

			for (rapidjson::SizeType i = 0; i < pointData.Size(); i++) {
				if (!pointData[i].IsObject()) {
					if (uiComponent) uiComponent->displayWarningPrompt("Sphere json document is broke: Entry is not an object.");
					else assert(0);
					return;
				}

				auto ptObj = pointData[i].GetObject();
				if (!ptObj.HasMember(RBE_JSON_Point_ID)) {
					if (uiComponent) uiComponent->displayWarningPrompt("Sphere json document is broken : Member \"" RBE_JSON_Point_ID "\" is missing");
					else assert(0);
					return;
				}
				if (!ptObj.HasMember(RBE_JSON_Point_U)) {
					if (uiComponent) uiComponent->displayWarningPrompt("Sphere json document is broken : Member \"" RBE_JSON_Point_U "\" is missing");
					else assert(0);
					return;
				}
				if (!ptObj.HasMember(RBE_JSON_Point_V)) {
					if (uiComponent) uiComponent->displayWarningPrompt("Sphere json document is broken : Member \"" RBE_JSON_Point_V "\" is missing");
					else assert(0);
					return;
				}
				if (!ptObj.HasMember(RBE_JSON_Point_W)) {
					if (uiComponent) uiComponent->displayWarningPrompt("Sphere json document is broken : Member \"" RBE_JSON_Point_W "\" is missing");
					else assert(0);
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
					p1Set = true;
				}
				if (pId == 2) {
					p2.SetX(pX);
					p2.SetY(pY);
					p2.SetZ(pZ);
					p2Set = true;
				}
				if (pId == 3) {
					p3.SetX(pX);
					p3.SetY(pY);
					p3.SetZ(pZ);
					p3Set = true;
				}
				if (pId == 4) {
					p4.SetX(pX);
					p4.SetY(pY);
					p4.SetZ(pZ);
					p4Set = true;
				}
				if (pId == 5) {
					p5.SetX(pX);
					p5.SetY(pY);
					p5.SetZ(pZ);
					p5Set = true;
				}
				if (pId == 6) {
					p6.SetX(pX);
					p6.SetY(pY);
					p6.SetZ(pZ);
					p6Set = true;
				}
				if (pId == 7) {
					p7.SetX(pX);
					p7.SetY(pY);
					p7.SetZ(pZ);
					p7Set = true;
				}
			}

			if (!p1Set || !p2Set || !p3Set || !p4Set || !p5Set || !p6Set || !p7Set) {
				if (uiComponent) 
					uiComponent->displayWarningPrompt("Modeling: Not all points were provided for creating a sphere from json.");
				else
					assert(0); return;
			}

			double xcenter = p1.X();
			double ycenter = p1.Y();
			double zcenter = p1.Z();

			double x = p1.X() - p2.X();
			double y = p1.Y() - p2.Y();
			double z = p1.Z() - p2.Z();
			double radius = sqrt(x*x + y*y + z*z);

			TopoDS_Shape body = BRepPrimAPI_MakeSphere(gp_Pnt(xcenter, ycenter, zcenter), radius);

			std::list<std::pair<std::string, std::string>> shapeParameters;
			shapeParameters.push_back(std::pair<std::string, std::string>("XCenter", to_string(xcenter)));
			shapeParameters.push_back(std::pair<std::string, std::string>("YCenter", to_string(ycenter)));
			shapeParameters.push_back(std::pair<std::string, std::string>("ZCenter", to_string(zcenter)));
			shapeParameters.push_back(std::pair<std::string, std::string>("Radius", to_string(radius)));

			std::list<std::string> faceNames = { "f1" };

			storeShapeInModel(body, _transform, "Geometry/Sphere", "Sphere", shapeParameters, faceNames);
		}
		else {
			if (uiComponent) 
				uiComponent->displayWarningPrompt("Invalid number of points received for creating a sphere from json.");
			else
				assert(0);
		}
	}
	else {
		// Document is not an array
		assert(0);
	}
}

void PrimitiveSphere::update(EntityGeometry *geomEntity, TopoDS_Shape &shape)
{
	EntityPropertiesDouble *xCenterProperty = dynamic_cast<EntityPropertiesDouble *>(geomEntity->getProperties().getProperty("#XCenter"));
	EntityPropertiesDouble *yCenterProperty = dynamic_cast<EntityPropertiesDouble *>(geomEntity->getProperties().getProperty("#YCenter"));
	EntityPropertiesDouble *zCenterProperty = dynamic_cast<EntityPropertiesDouble *>(geomEntity->getProperties().getProperty("#ZCenter"));
	EntityPropertiesDouble *radiusProperty  = dynamic_cast<EntityPropertiesDouble *>(geomEntity->getProperties().getProperty("#Radius"));

	if (xCenterProperty == nullptr || yCenterProperty == nullptr || zCenterProperty == nullptr || radiusProperty == nullptr) {
		assert(0);
		return;
	}

	double xcenter = xCenterProperty->getValue();
	double ycenter = yCenterProperty->getValue();
	double zcenter = zCenterProperty->getValue();
	double radius = radiusProperty->getValue();

	try {
		shape = BRepPrimAPI_MakeSphere(gp_Pnt(xcenter, ycenter, zcenter), radius);
	}
	catch (Standard_Failure) {
		assert(0);
	}
}
