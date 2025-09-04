#include "PrimitivePyramid.h"

#include "EntityGeometry.h"

#include "OTServiceFoundation/UiComponent.h"

#include <rbeCore/RubberbandConfiguration.h>
#include <rbeCore/jsonMember.h>

#include <cassert>
#include <algorithm>

#include <BRepPrimAPI_MakeWedge.hxx>

void PrimitivePyramid::sendRubberbandData(void) 
{
	if (uiComponent == nullptr) {
		assert(0);
		return;
	}

	std::string cfgString;

	rbeCore::RubberbandConfiguration rubberbandConfig;
	rubberbandConfig.setToDefaultPyramid();
	cfgString = rubberbandConfig.toJson();
	if (cfgString.empty()) {
		assert("Rubberband is empty");
	}

	uiComponent->createRubberband("ModelingService.Pyramid", cfgString);
}

void PrimitivePyramid::createFromRubberbandJson(const std::string& _json, std::vector<double> &_transform) 
{
	rapidjson::Document doc;
	doc.Parse(_json.c_str());

	if (doc.IsArray()) {
		auto pntData = doc.GetArray();
		if (pntData.Size() == 6) {
			gp_Pnt p1;
			gp_Pnt p2;
			gp_Pnt p3;
			gp_Pnt p4;
			gp_Pnt p5;
			gp_Pnt p6;

			bool pnt1Set = false;
			bool pnt2Set = false;
			bool pnt3Set = false;
			bool pnt4Set = false;
			bool pnt5Set = false;
			bool pnt6Set = false;

			for (rapidjson::SizeType i = 0; i < pntData.Size(); i++) {
				if (!pntData[i].IsObject()) {
					if (uiComponent)
						uiComponent->displayWarningPrompt("Pyramid entry document in broken. Entry is not a object.");
					else
						assert(0);

					return;
				}

				auto pntObject = pntData[i].GetObject();
				if (!pntObject.HasMember(RBE_JSON_Point_ID)) {
					if (uiComponent)
						uiComponent->displayWarningPrompt("Pyramid obejct is broke: Member \"" RBE_JSON_Point_ID "\" is missing");
					else
						assert(0);

					return;
				}

				if (!pntObject.HasMember(RBE_JSON_Point_U)) {
					if (uiComponent)
						uiComponent->displayWarningPrompt("Pyramid obejct is broke: Member \"" RBE_JSON_Point_U "\" is missing");
					else
						assert(0);

					return;
				}

				if (!pntObject.HasMember(RBE_JSON_Point_V)) {
					if (uiComponent)
						uiComponent->displayWarningPrompt("Pyramid obejct is broke: Member \"" RBE_JSON_Point_V "\" is missing");
					else
						assert(0);

					return;
				}

				if (!pntObject.HasMember(RBE_JSON_Point_W)) {
					if (uiComponent)
						uiComponent->displayWarningPrompt("Pyramid obejct is broke: Member \"" RBE_JSON_Point_W "\" is missing");
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
			}

			if (!pnt1Set || !pnt2Set || !pnt3Set || !pnt4Set || !pnt5Set || !pnt6Set) {
				if (uiComponent)
					uiComponent->displayWarningPrompt("Modeling: Not all points were provided to create a pyramid from json;");
				else
					assert(0);
			}

			double xmin = std::min(p1.X(), std::min(p2.X(), std::min(p3.X(), p4.X())));
			double xmax = std::max(p1.X(), std::max(p2.X(), std::max(p3.X(), p4.X())));
			double ymin = std::min(p1.Y(), std::min(p2.Y(), std::min(p3.Y(), p4.Y())));
			double ymax = std::max(p1.Y(), std::max(p2.Y(), std::max(p3.Y(), p4.Y())));
			double zmin = p5.Z();
			double zmax = p6.Z();

			double dx = xmax - xmin;
			double dy = ymax - ymin;
			double dz = zmax - zmin;

			std::list<std::string> faceNames;
			TopoDS_Shape pyramid;

			if (dx > Precision::Confusion() && dy > Precision::Confusion() && fabs(dz) > Precision::Confusion())
			{
				gp_Ax2 axis;
				if (zmin < zmax) {
					axis = gp_Ax2(gp_Pnt(xmin, ymin, zmin), gp_Dir(1, 0, 0), gp_Dir(0, 1, 0));
					std::swap(dx, dy);
				}
				else {
					axis = gp_Ax2(gp_Pnt(xmin, ymin, zmin), gp_Dir(0, 1, 0), gp_Dir(1, 0, 0));
					dz *= (-1);
				}
				pyramid = BRepPrimAPI_MakeWedge(axis, dx, dz, dy, dx / 2, dy / 2, dx / 2, dy / 2);
				faceNames = { "f1", "f2", "f3", "f4", "f5" };
			}

			std::list<std::pair<std::string, std::string>> shapeParameters;
			shapeParameters.push_back(std::pair<std::string, std::string>("XMin", to_string(xmin)));
			shapeParameters.push_back(std::pair<std::string, std::string>("XMax", to_string(xmax)));
			shapeParameters.push_back(std::pair<std::string, std::string>("YMin", to_string(ymin)));
			shapeParameters.push_back(std::pair<std::string, std::string>("YMax", to_string(ymax)));
			shapeParameters.push_back(std::pair<std::string, std::string>("ZMin", to_string(zmin)));
			shapeParameters.push_back(std::pair<std::string, std::string>("ZMax", to_string(zmax)));

			storeShapeInModel(pyramid, _transform, "Geometry/Pyramid", "Pyramid", shapeParameters, faceNames);
		}
		else {
			if (uiComponent)
				uiComponent->displayWarningPrompt("Invalid number of points received from json.");
			else
				assert(0); return;
		}
	}
	else {
		assert(0);
	}
}

void PrimitivePyramid::update(EntityGeometry *geomEntity, TopoDS_Shape &shape)
{
	EntityPropertiesDouble *XMinProperty = dynamic_cast<EntityPropertiesDouble *>(geomEntity->getProperties().getProperty("#XMin"));
	EntityPropertiesDouble *XMaxProperty = dynamic_cast<EntityPropertiesDouble *>(geomEntity->getProperties().getProperty("#XMax"));
	EntityPropertiesDouble *YMinProperty = dynamic_cast<EntityPropertiesDouble *>(geomEntity->getProperties().getProperty("#YMin"));
	EntityPropertiesDouble *YMaxProperty = dynamic_cast<EntityPropertiesDouble *>(geomEntity->getProperties().getProperty("#YMax"));
	EntityPropertiesDouble *ZMinProperty = dynamic_cast<EntityPropertiesDouble *>(geomEntity->getProperties().getProperty("#ZMin"));
	EntityPropertiesDouble *ZMaxProperty = dynamic_cast<EntityPropertiesDouble *>(geomEntity->getProperties().getProperty("#ZMax"));

	if (XMinProperty == nullptr || XMaxProperty == nullptr || YMinProperty == nullptr || YMaxProperty == nullptr 
		|| ZMinProperty == nullptr || ZMaxProperty == nullptr)
	{
		assert(0);
		return;
	}

	double xmin = XMinProperty->getValue();
	double xmax = XMaxProperty->getValue();
	double ymin = YMinProperty->getValue();
	double ymax = YMaxProperty->getValue();
	double zmin = ZMinProperty->getValue();
	double zmax = ZMaxProperty->getValue();

	double dx = (xmax - xmin) < 0 ? (xmax - xmin)*(-1) : (xmax - xmin);
	double dy = (ymax - ymin) < 0 ? (ymax - ymin)*(-1) : (ymax - ymin);
	double dz = zmax - zmin;

	if (xmin > xmax) {
		double temp = xmin;
		xmin = xmax;
		xmax = temp;
	}
	if (ymin > ymax) {
		double temp = ymin;
		ymin = ymax;
		ymax = temp;
	}

	std::list<std::string> faceNames;

	if (dx > Precision::Confusion() && dy > Precision::Confusion() && fabs(dz) > Precision::Confusion())
	{
		gp_Ax2 axis;
		if (zmin < zmax) {
			axis = gp_Ax2(gp_Pnt(xmin, ymin, zmin), gp_Dir(1, 0, 0), gp_Dir(0, 1, 0));
			std::swap(dx, dy);
		}
		else {
			axis = gp_Ax2(gp_Pnt(xmin, ymin, zmin), gp_Dir(0, 1, 0), gp_Dir(1, 0, 0));
			dz *= (-1);
		}

		try {
			shape = BRepPrimAPI_MakeWedge(axis, dx, dz, dy, dx / 2, dy / 2, dx / 2, dy / 2);
			faceNames = { "f1", "f2", "f3", "f4", "f5" };
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
