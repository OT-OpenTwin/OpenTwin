// @otlicense

#include "PrimitiveCone.h"

#include "EntityGeometry.h"

#include "OTServiceFoundation/UiComponent.h"

#include <rbeCore/RubberbandConfiguration.h>
#include <rbeCore/jsonMember.h>

#include <cassert>
#include <algorithm>

#include <BRepPrimAPI_MakeCone.hxx>
#include "TopExp_Explorer.hxx"

void PrimitiveCone::sendRubberbandData(void) 
{
	if (uiComponent == nullptr) {
		assert(0);
		return;
	}

	std::string cfgString;

	rbeCore::RubberbandConfiguration rubberbandConfig;
	rubberbandConfig.setToDefaultCone();
	cfgString = rubberbandConfig.toJson();
	if (cfgString.empty()) {
		assert("Rubberband is empty");
	}

	uiComponent->createRubberband("ModelingService.Cone", cfgString);
}

void PrimitiveCone::createFromRubberbandJson(const std::string& _json, std::vector<double> &_transform) 
{
	rapidjson::Document doc;
	doc.Parse(_json.c_str());

	if (doc.IsArray()) {
		auto pointData = doc.GetArray();
		if (pointData.Size() == 18) {
			gp_Pnt p1;
			gp_Pnt p2;
			gp_Pnt p3;
			gp_Pnt p4;
			gp_Pnt p5;
			gp_Pnt p6;
			gp_Pnt p11;
			gp_Pnt p12;
			gp_Pnt p13;
			gp_Pnt p14;

			bool pnt1Set = false;
			bool pnt2Set = false;
			bool pnt3Set = false;
			bool pnt4Set = false;
			bool pnt5Set = false;
			bool pnt6Set = false;
			bool pnt11Set = false;
			bool pnt12Set = false;
			bool pnt13Set = false;
			bool pnt14Set = false;

			for (rapidjson::SizeType i = 0; i < pointData.Size(); i++) {
				if (!pointData[i].IsObject()) {
					if (uiComponent)
						uiComponent->displayWarningPrompt("Cone entry document is broke: Entry is not a object.");
					else
						assert(0); 

					return;
				}

				auto pntObject = pointData[i].GetObject();
				if (!pntObject.HasMember(RBE_JSON_Point_ID)) {
					if (uiComponent)
						uiComponent->displayWarningPrompt("Cone member json is broke: Member \"" RBE_JSON_Point_ID "\" is missing.");
					else
						assert(0);

					return;
				}

				if (!pntObject.HasMember(RBE_JSON_Point_U)) {
					if (uiComponent)
						uiComponent->displayWarningPrompt("Cone member json is broke: Member \"" RBE_JSON_Point_U "\" is missing.");
					else
						assert(0);

					return;
				}

				if (!pntObject.HasMember(RBE_JSON_Point_V)) {
					if (uiComponent)
						uiComponent->displayWarningPrompt("Cone member json is broke: Member \"" RBE_JSON_Point_V "\" is missing.");
					else
						assert(0);

					return;
				}

				if (!pntObject.HasMember(RBE_JSON_Point_W)) {
					if (uiComponent)
						uiComponent->displayWarningPrompt("Cone member json is broke: Member \"" RBE_JSON_Point_W "\" is missing.");
					else
						assert(0);

					return;
				}

				int pID = pntObject[RBE_JSON_Point_ID].GetInt();
				double pX = pntObject[RBE_JSON_Point_U].GetDouble();
				double pY = pntObject[RBE_JSON_Point_V].GetDouble();
				double pZ = pntObject[RBE_JSON_Point_W].GetDouble();

				if (pID == 1) {
					p1.SetX(pX);
					p1.SetY(pY);
					p1.SetZ(pZ);
					pnt1Set = true;
				}
				if (pID == 2) {
					p2.SetX(pX);
					p2.SetY(pY);
					p2.SetZ(pZ);
					pnt2Set = true;
				}
				if (pID == 3) {
					p3.SetX(pX);
					p3.SetY(pY);
					p3.SetZ(pZ);
					pnt3Set = true;
				}
				if (pID == 4) {
					p4.SetX(pX);
					p4.SetY(pY);
					p4.SetZ(pZ);
					pnt4Set = true;
				}
				if (pID == 5) {
					p5.SetX(pX);
					p5.SetY(pY);
					p5.SetZ(pZ);
					pnt5Set = true;
				}
				if (pID == 6) {
					p6.SetX(pX);
					p6.SetY(pY);
					p6.SetZ(pZ);
					pnt6Set = true;
				}
				if (pID == 11) {
					p11.SetX(pX);
					p11.SetY(pY);
					p11.SetZ(pZ);
					pnt11Set = true;
				}
				if (pID == 12) {
					p12.SetX(pX);
					p12.SetY(pY);
					p12.SetZ(pZ);
					pnt12Set = true;
				}
				if (pID == 13) {
					p13.SetX(pX);
					p13.SetY(pY);
					p13.SetZ(pZ);
					pnt13Set = true;
				}
				if (pID == 14) {
					p14.SetX(pX);
					p14.SetY(pY);
					p14.SetZ(pZ);
					pnt14Set = true;
				}
			}

			if (!pnt1Set || !pnt2Set || !pnt3Set || !pnt4Set || !pnt5Set || !pnt6Set || !pnt11Set || !pnt12Set || !pnt13Set || !pnt14Set) {
				if (uiComponent)
					uiComponent->displayWarningPrompt("Modeling: Not all points were provided for creating a cone from json.");
				else
					assert(0); return;
			}

			double xcenter = p1.X();
			double ycenter = p1.Y();
			double zmin	   = p1.Z();
			double zmax    = p6.Z();

			// calculating radius of of the bottom circle
			double bottomRadiusX = p2.X() - xcenter;
			double bottomRadiusY = p2.Y() - ycenter;
			double bottomRadiusZ = p2.Z() - zmin;
			double bottomRadius = sqrt(bottomRadiusX*bottomRadiusX + bottomRadiusY*bottomRadiusY + bottomRadiusZ*bottomRadiusZ);

			// calculating radius of top circle
			double topRadiusX = p6.X() - p11.X();
			double topRadiusY = p6.Y() - p11.Y();
			double topRadiusZ = p6.Z() - p11.Z();
			double topRadius = sqrt(topRadiusX*topRadiusX + topRadiusY*topRadiusY + topRadiusZ*topRadiusZ);

			gp_Pnt centerPoint(xcenter, ycenter, zmin);
			gp_Dir direction;
			TopoDS_Shape body;
			std::list<std::string> faceNames;

			if (fabs(zmax - zmin) > Precision::Confusion()
				&& bottomRadius >= 0
				&& topRadius >= 0
				&& !(bottomRadius < Precision::Confusion() && topRadius < Precision::Confusion()))
			{
				if (zmax > zmin) {
					direction = gp_Dir(0, 0, 1);
					body = BRepPrimAPI_MakeCone(gp_Ax2(centerPoint, direction), bottomRadius, topRadius, zmax);
				}
				else {
					direction = gp_Dir(0, 0, -1);
					zmax = fabs(zmax);
					body = BRepPrimAPI_MakeCone(gp_Ax2(centerPoint, direction), bottomRadius, topRadius, zmax);
					zmax *= (-1);
				}

				TopExp_Explorer exp;
				size_t faceCount = 0;
				for (exp.Init(body, TopAbs_FACE); exp.More(); exp.Next()) faceCount++;

				if (faceCount == 2)
				{
					// Either top or bottom radii are zero, so we have a complete cone
					faceNames = { "f1", "f2" };
				}
				else
				{
					// Both, top and bottom radii are non-zero, so we have a truncated cone
					faceNames = { "f1", "f2", "f3" };
				}
			}

			std::list<std::pair<std::string, std::string>> shapeParameters;
			shapeParameters.push_back(std::pair<std::string, std::string>("XCenter",	  to_string(xcenter)));
			shapeParameters.push_back(std::pair<std::string, std::string>("YCenter",	  to_string(ycenter)));
			shapeParameters.push_back(std::pair<std::string, std::string>("ZMin",		  to_string(zmin)));
			shapeParameters.push_back(std::pair<std::string, std::string>("ZMax",		  to_string(zmax)));
			shapeParameters.push_back(std::pair<std::string, std::string>("BottomRadius", to_string(bottomRadius)));
			shapeParameters.push_back(std::pair<std::string, std::string>("TopRadius",	  to_string(topRadius)));

			storeShapeInModel(body, _transform, "Geometry/Cone", "Cone", shapeParameters, faceNames);
		}
		else {
			if (uiComponent)
				uiComponent->displayWarningPrompt("Invalid number of points were received from json");
			else
				assert(0); return;
		}
	}
	else {
		assert(0);
	}
}

void PrimitiveCone::update(EntityGeometry *geomEntity, TopoDS_Shape &shape)
{
	EntityPropertiesDouble *xCenterProperty		 = dynamic_cast<EntityPropertiesDouble *>(geomEntity->getProperties().getProperty("#XCenter"));
	EntityPropertiesDouble *yCenterProperty		 = dynamic_cast<EntityPropertiesDouble *>(geomEntity->getProperties().getProperty("#YCenter"));
	EntityPropertiesDouble *zMinProperty		 = dynamic_cast<EntityPropertiesDouble *>(geomEntity->getProperties().getProperty("#ZMin"));
	EntityPropertiesDouble *zMaxProperty		 = dynamic_cast<EntityPropertiesDouble *>(geomEntity->getProperties().getProperty("#ZMax"));
	EntityPropertiesDouble *bottomRadiusProperty = dynamic_cast<EntityPropertiesDouble *>(geomEntity->getProperties().getProperty("#BottomRadius"));
	EntityPropertiesDouble *topRadiusProperty	 = dynamic_cast<EntityPropertiesDouble *>(geomEntity->getProperties().getProperty("#TopRadius"));

	if (xCenterProperty == nullptr || yCenterProperty == nullptr || zMinProperty == nullptr || zMaxProperty == nullptr
		|| bottomRadiusProperty == nullptr || topRadiusProperty == nullptr)
	{
		assert(0);
		return;
	}

	double xcenter		= xCenterProperty->getValue();
	double ycenter		= yCenterProperty->getValue();
	double zMin			= zMinProperty->getValue();
	double zMax			= zMaxProperty->getValue();
	double bottomRadius = bottomRadiusProperty->getValue();
	double topRadius	= topRadiusProperty->getValue();

	gp_Pnt centerPoint = gp_Pnt(xcenter, ycenter, zMin);
	gp_Dir direction;
	std::list<std::string> faceNames;

	if (fabs(zMax - zMin) > Precision::Confusion()
		&& bottomRadius >= 0
		&& topRadius >= 0
		&& !(bottomRadius < Precision::Confusion() && topRadius < Precision::Confusion()))
	{
		try {
			if (zMax > zMin) {
				direction = gp_Dir(0, 0, 1);
				shape = BRepPrimAPI_MakeCone(gp_Ax2(centerPoint, direction), bottomRadius, topRadius, fabs(zMax - zMin));
			}
			else {
				direction = gp_Dir(0, 0, -1);
				shape = BRepPrimAPI_MakeCone(gp_Ax2(centerPoint, direction), bottomRadius, topRadius, fabs(zMax - zMin));
			}

			TopExp_Explorer exp;
			size_t faceCount = 0;
			for (exp.Init(shape, TopAbs_FACE); exp.More(); exp.Next()) faceCount++;

			if (faceCount == 2)
			{
				// Either top or bottom radii are zero, so we have a complete cone
				faceNames = { "f1", "f2" };
			}
			else
			{
				// Both, top and bottom radii are non-zero, so we have a truncated cone
				faceNames = { "f1", "f2", "f3" };
			}
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
