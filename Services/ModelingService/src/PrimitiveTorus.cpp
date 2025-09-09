#include "PrimitiveTorus.h"

#include "EntityGeometry.h"

#include "OTServiceFoundation/UiComponent.h"

#include <rbeCore/RubberbandConfiguration.h>
#include <rbeCore/jsonMember.h>

#include <cassert>
#include <algorithm>

#include <BRepPrimAPI_MakeTorus.hxx>

void PrimitiveTorus::sendRubberbandData(void) 
{
	if (uiComponent == nullptr) {
		assert(0);
		return;
	}

	std::string cfgString;

	rbeCore::RubberbandConfiguration rubberbandConfig;
	rubberbandConfig.setToDefaultTorus();
	cfgString = rubberbandConfig.toJson();
	if (cfgString.empty())
	{
		assert("Rubberband is empty.");
	}

	uiComponent->createRubberband("ModelingService.Torus", cfgString);
}

void PrimitiveTorus::createFromRubberbandJson(const std::string& _json, std::vector<double> &_transform) 
{
	rapidjson::Document doc;
	doc.Parse(_json.c_str());

	if (doc.IsArray()) {
		auto pointData = doc.GetArray();
		if (pointData.Size() == 13) {
			gp_Pnt p1;
			gp_Pnt p2;
			gp_Pnt p3;
			gp_Pnt p4;
			gp_Pnt p5;
			gp_Pnt p6;
			gp_Pnt p7;
			gp_Pnt p8;
			gp_Pnt p9;
			gp_Pnt p206;
			gp_Pnt p307;
			gp_Pnt p408;
			gp_Pnt p509;

			bool pnt1Set = false;
			bool pnt2Set = false;
			bool pnt3Set = false;
			bool pnt4Set = false;
			bool pnt5Set = false;
			bool pnt6Set = false;
			bool pnt7Set = false;
			bool pnt8Set = false;
			bool pnt9Set = false;
			bool pnt206Set = false;
			bool pnt307Set = false;
			bool pnt408Set = false;
			bool pnt509Set = false;

			for (rapidjson::SizeType i = 0; i < pointData.Size(); i++) {
				if (!pointData[i].IsObject()) {
					if (uiComponent)
						uiComponent->displayWarningPrompt("Torus entry document is broke: Entry is not a object.");
					else
						assert(0);

					return;
				}

				auto pntObject = pointData[i].GetObject();
				if (!pntObject.HasMember(RBE_JSON_Point_ID)) {
					if (uiComponent)
						uiComponent->displayWarningPrompt("Torus json document is brooken: Member \"" RBE_JSON_Point_ID "\" is missing.");
					else
						assert(0);

					return;
				}

				if (!pntObject.HasMember(RBE_JSON_Point_U)) {
					if (uiComponent)
						uiComponent->displayWarningPrompt("Torus json document is broken: Member \"" RBE_JSON_Point_U "\" is missing.");
					else
						assert(0); 

					return;
				}

				if (!pntObject.HasMember(RBE_JSON_Point_V)) {
					if (uiComponent)
						uiComponent->displayWarningPrompt("Torus json document is broken: Member \"" RBE_JSON_Point_V "\" is missing.");
					else
						assert(0);

					return;
				}

				if (!pntObject.HasMember(RBE_JSON_Point_W)) {
					if (uiComponent)
						uiComponent->displayWarningPrompt("Torus json document is broken: Member \"" RBE_JSON_Point_W "\" is missing.");
					else
						assert(0);

					return;
				}

				int pID = pntObject[RBE_JSON_Point_ID].GetInt();
				double pntX = pntObject[RBE_JSON_Point_U].GetDouble();
				double pntY = pntObject[RBE_JSON_Point_V].GetDouble();
				double pntZ = pntObject[RBE_JSON_Point_W].GetDouble();

				if (pID == 1) {
					p1.SetX(pntX);
					p1.SetY(pntY);
					p1.SetZ(pntZ);
					pnt1Set = true;
				}
				if (pID == 2) {
					p2.SetX(pntX);
					p2.SetY(pntY);
					p2.SetZ(pntZ);
					pnt2Set = true;
				}
				if (pID == 3) {
					p3.SetX(pntX);
					p3.SetY(pntY);
					p3.SetZ(pntZ);
					pnt3Set = true;
				}
				if (pID == 4) {
					p4.SetX(pntX);
					p4.SetY(pntY);
					p4.SetZ(pntZ);
					pnt4Set = true;
				}
				if (pID == 5) {
					p5.SetX(pntX);
					p5.SetY(pntY);
					p5.SetZ(pntZ);
					pnt5Set = true;
				}
				if (pID == 6) {
					p6.SetX(pntX);
					p6.SetY(pntY);
					p6.SetZ(pntZ);
					pnt6Set = true;
				}
				if (pID == 7) {
					p7.SetX(pntX);
					p7.SetY(pntY);
					p7.SetZ(pntZ);
					pnt7Set = true;
				}
				if (pID == 8) {
					p8.SetX(pntX);
					p8.SetY(pntY);
					p8.SetZ(pntZ);
					pnt8Set = true;
				}
				if (pID == 9) {
					p9.SetX(pntX);
					p9.SetY(pntY);
					p9.SetZ(pntZ);
					pnt9Set = true;
				}
				if (pID == 206) {
					p206.SetX(pntX);
					p206.SetY(pntY);
					p206.SetZ(pntZ);
					pnt206Set = true;
				}
				if (pID == 307) {
					p307.SetX(pntX);
					p307.SetY(pntY);
					p307.SetZ(pntZ);
					pnt307Set = true;
				}
				if (pID == 408) {
					p408.SetX(pntX);
					p408.SetY(pntY);
					p408.SetZ(pntZ);
					pnt408Set = true;
				}
				if (pID == 509) {
					p509.SetX(pntX);
					p509.SetY(pntY);
					p509.SetZ(pntZ);
					pnt509Set = true;
				}
			}

			if (!pnt1Set || !pnt2Set || !pnt3Set || !pnt4Set || !pnt5Set || !pnt6Set || !pnt7Set || !pnt8Set 
				|| !pnt9Set || !pnt206Set || !pnt307Set || !pnt408Set || !pnt509Set) 
			{
				if (uiComponent)
					uiComponent->displayWarningPrompt("Modeling: Not all points were provided for creating a torus from json.");
				else
					assert(0); return;
			}

			double xcenter = p1.X();
			double ycenter = p1.Y();
			double zcenter = p1.Z();

			// vector from point 1 to point 206
			double distanceToPipeCenterX = p206.X() - p1.X();
			double distanceToPipeCenterY = p206.Y() - p1.Y();
			double distanceToPipeCenterZ = p206.Z() - p1.Z();
			double distanceToPipeCenter = sqrt((distanceToPipeCenterX*distanceToPipeCenterX) + (distanceToPipeCenterY*distanceToPipeCenterY) + (distanceToPipeCenterZ*distanceToPipeCenterZ));

			// vector from point 6 to point 206
			double pipeRadiusX = p6.X() - p206.X();
			double pipeRadiusY = p6.Y() - p206.Y();
			double pipeRadiusZ = p6.Z() - p206.Z();
			double pipeRadius = sqrt((pipeRadiusX*pipeRadiusX) + (pipeRadiusY*pipeRadiusY) + (pipeRadiusZ*pipeRadiusZ));

			// vector from point 1 to point 2
			double distanceToInnerCircleX = p1.X() - p2.X();
			double distanceToInnerCircleY = p1.Y() - p2.Y();
			double distanceToInnerCircleZ = p1.Z() - p2.Z();
			double distanceToInnerCircle = sqrt((distanceToInnerCircleX*distanceToInnerCircleX) + (distanceToInnerCircleY*distanceToInnerCircleY) + (distanceToInnerCircleZ*distanceToInnerCircleZ));

			// vector from point 1 to point 2
			double distanceToOuterCircleX = p1.X() - p6.X();
			double distanceToOuterCircleY = p1.Y() - p6.Y();
			double distanceToOuterCircleZ = p1.Z() - p6.Z();
			double distanceToOuterCircle = sqrt((distanceToOuterCircleX*distanceToOuterCircleX) + (distanceToOuterCircleY*distanceToOuterCircleY) + (distanceToOuterCircleZ*distanceToOuterCircleZ));

			// creating the torus
			TopoDS_Shape body;
			std::list<std::string> faceNames;

			if (distanceToPipeCenter > Precision::Confusion() && pipeRadius > Precision::Confusion() && pipeRadius <= distanceToPipeCenter)
			{
				body = BRepPrimAPI_MakeTorus(gp_Ax2(gp_Pnt(xcenter, ycenter, zcenter), gp_Dir(0, 0, 1)), distanceToPipeCenter, pipeRadius);
				faceNames = { "f1" };
			}

			std::list<std::pair<std::string, std::string>> shapeParameters;
			shapeParameters.push_back(std::pair<std::string, std::string>("XCenter",					     to_string(xcenter)));
			shapeParameters.push_back(std::pair<std::string, std::string>("YCenter",					     to_string(ycenter)));
			shapeParameters.push_back(std::pair<std::string, std::string>("ZCenter",					     to_string(zcenter)));
			shapeParameters.push_back(std::pair<std::string, std::string>("Distance center to pipe center",  to_string(distanceToPipeCenter)));
			shapeParameters.push_back(std::pair<std::string, std::string>("Pipe radius",				     to_string(pipeRadius)));

			storeShapeInModel(body, _transform, "Geometry/Torus", "Torus", shapeParameters, faceNames);
		}
		else {
			if (uiComponent)
				uiComponent->displayWarningPrompt("Invalid number of points received from json.");
			else
				assert(0); return;
		}
	}
	else {
		assert(0); return;
	}
}

void PrimitiveTorus::update(EntityGeometry *geomEntity, TopoDS_Shape &shape)
{
	EntityPropertiesDouble *xCenterProperty				 = dynamic_cast<EntityPropertiesDouble *>(geomEntity->getProperties().getProperty("#XCenter"));
	EntityPropertiesDouble *yCenterProperty				 = dynamic_cast<EntityPropertiesDouble *>(geomEntity->getProperties().getProperty("#YCenter"));
	EntityPropertiesDouble *zCenterProperty				 = dynamic_cast<EntityPropertiesDouble *>(geomEntity->getProperties().getProperty("#ZCenter"));
	EntityPropertiesDouble *distanceToPipeCenterProperty = dynamic_cast<EntityPropertiesDouble *>(geomEntity->getProperties().getProperty("#Distance center to pipe center"));
	EntityPropertiesDouble *pipeRadiusProperty			 = dynamic_cast<EntityPropertiesDouble *>(geomEntity->getProperties().getProperty("#Pipe radius"));

	if (xCenterProperty == nullptr || yCenterProperty == nullptr || zCenterProperty == nullptr || distanceToPipeCenterProperty == nullptr
		|| pipeRadiusProperty == nullptr) {
		assert(0);
		return;
	}

	double xCenter				= xCenterProperty->getValue();
	double yCenter				= yCenterProperty->getValue();
	double zCenter				= zCenterProperty->getValue();
	double distanceToPipeCenter = distanceToPipeCenterProperty->getValue();
	double pipeRadius			= pipeRadiusProperty->getValue();

	std::list<std::string> faceNames;
	
	if (distanceToPipeCenter > Precision::Confusion() && pipeRadius > Precision::Confusion() && pipeRadius <= distanceToPipeCenter)
	{
		try {
			shape = BRepPrimAPI_MakeTorus(gp_Ax2(gp_Pnt(xCenter, yCenter, zCenter), gp_Dir(0, 0, 1)), distanceToPipeCenter, pipeRadius);
			faceNames = { "f1" };
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
