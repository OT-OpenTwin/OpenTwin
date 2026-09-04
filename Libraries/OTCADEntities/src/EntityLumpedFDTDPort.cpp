// @otlicense
// File: EntityLumpedFDTDPort.cpp
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

// OpenTwin header
#include "OTCommunication/ActionTypes.h"
#include "OTCADEntities/EntityLumpedFDTDPort.h"

// OpenCASCADE header
#include "TopoDS_Shape.hxx"

#include "BRepBuilderAPI_MakeFace.hxx"
#include "BRepBuilderAPI_MakePolygon.hxx"
#include "BRepPrimAPI_MakeBox.hxx"
#include "BRepPrimAPI_MakeCylinder.hxx"

#include "gp_Ax2.hxx"
#include "gp_Dir.hxx"
#include "gp_Pnt.hxx"
#include "Precision.hxx"

#include <algorithm>
#include <cmath>

static EntityFactoryRegistrar<EntityLumpedFDTDPort> registrar(EntityLumpedFDTDPort::className());

EntityLumpedFDTDPort::EntityLumpedFDTDPort(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms) :
	EntityGeometry(ID, parent, obs, ms)
{
	createProperties();
}

EntityLumpedFDTDPort::~EntityLumpedFDTDPort()
{
}

void EntityLumpedFDTDPort::addStorageData(bsoncxx::builder::basic::document& storage)
{
    // We store the parent class information first 
    EntityGeometry::addStorageData(storage);

    // Now we store the particular information about the current object

    storage.append(
        bsoncxx::builder::basic::kvp("TextString", textString),
        bsoncxx::builder::basic::kvp("TextPosX", textPosition[0]),
        bsoncxx::builder::basic::kvp("TextPosY", textPosition[1]),
        bsoncxx::builder::basic::kvp("TextPosZ", textPosition[2]),
        bsoncxx::builder::basic::kvp("TextNormalX", textNormal[0]),
        bsoncxx::builder::basic::kvp("TextNormalY", textNormal[1]),
        bsoncxx::builder::basic::kvp("TextNormalZ", textNormal[2]),
        bsoncxx::builder::basic::kvp("TextDirUX", textDirU[0]),
        bsoncxx::builder::basic::kvp("TextDirUY", textDirU[1]),
        bsoncxx::builder::basic::kvp("TextDirUZ", textDirU[2])
    );
}

void EntityLumpedFDTDPort::readSpecificDataFromDataBase(const bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
    // We read the parent class information first 
    EntityGeometry::readSpecificDataFromDataBase(doc_view, entityMap);

    clearText();
    if (doc_view["TextString"])
    {
        textString = doc_view["TextString"].get_string();
        textPosition = { doc_view["TextPosX"].get_double(), doc_view["TextPosY"].get_double(), doc_view["TextPosZ"].get_double() };
        textNormal = { doc_view["TextNormalX"].get_double(), doc_view["TextNormalY"].get_double(), doc_view["TextNormalZ"].get_double() };
        textDirU = { doc_view["TextDirUX"].get_double(), doc_view["TextDirUY"].get_double(), doc_view["TextDirUZ"].get_double() };
    }

    resetModified();
}

void EntityLumpedFDTDPort::clearText(void)
{
    textString.clear();
    textPosition = { 0.0, 0.0, 0.0 };
    textNormal = { 0.0, 0.0, 0.0 };
    textDirU = { 0.0, 0.0, 0.0 };
}

void EntityLumpedFDTDPort::createProperties()
{
	EntityPropertiesDouble::createProperty("General", "Impedance", 50, "Lumped Ports", getProperties());
	EntityPropertiesColor::createProperty("General", "Color", { 255, 171, 0 }, "Lumped Ports", getProperties());
	EntityPropertiesSelection::createProperty("General", "Current direction", {"X", "Y", "Z"}, "Z", "Lumped Ports", getProperties());

	EntityPropertiesDouble::createProperty("Port range", "Xmin", 0, "Lumped Ports", getProperties())->setGroupChanges(true);
	EntityPropertiesDouble::createProperty("Port range", "Xmax", 0, "Lumped Ports", getProperties())->setGroupChanges(true);
	EntityPropertiesDouble::createProperty("Port range", "Ymin", 0, "Lumped Ports", getProperties())->setGroupChanges(true);
	EntityPropertiesDouble::createProperty("Port range", "Ymax", 0, "Lumped Ports", getProperties())->setGroupChanges(true);
	EntityPropertiesDouble::createProperty("Port range", "Zmin", 0, "Lumped Ports", getProperties())->setGroupChanges(true);
	EntityPropertiesDouble::createProperty("Port range", "Zmax", 0, "Lumped Ports", getProperties())->setGroupChanges(true);

    EntityPropertiesSelection::createProperty("Label", "Text normal", { "-X", "+X", "-Y", "+Y", "-Z", "+Z" }, "+Y", "Lumped Ports", getProperties());
    EntityPropertiesSelection::createProperty("Label", "Text direction", { "-X", "+X", "-Y", "+Y", "-Z", "+Z" }, "+Y", "Lumped Ports", getProperties());
}

TopoDS_Shape EntityLumpedFDTDPort::createShape(double lineRadius, double tolerance)
{
    getProperties().forceResetUpdateForAllProperties();

    EntityPropertiesDouble* xMinProperty = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("Xmin"));
    EntityPropertiesDouble* xMaxProperty = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("Xmax"));
    EntityPropertiesDouble* yMinProperty = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("Ymin"));
    EntityPropertiesDouble* yMaxProperty = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("Ymax"));
    EntityPropertiesDouble* zMinProperty = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("Zmin"));
    EntityPropertiesDouble* zMaxProperty = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("Zmax"));
    assert(xMinProperty && xMaxProperty && yMinProperty && yMaxProperty && zMinProperty && zMaxProperty);

    double xmin = xMinProperty->getValue();
    double xmax = xMaxProperty->getValue();
    double ymin = yMinProperty->getValue();
    double ymax = yMaxProperty->getValue();
    double zmin = zMinProperty->getValue();
    double zmax = zMaxProperty->getValue();

    // Normalize the bounds
    if (xmin > xmax)
        std::swap(xmin, xmax);
    if (ymin > ymax)
        std::swap(ymin, ymax);
    if (zmin > zmax)
        std::swap(zmin, zmax);

    determineLabelOrientation(xmin, xmax, ymin, ymax, zmin, zmax);

    const double dx = xmax - xmin;
    const double dy = ymax - ymin;
    const double dz = zmax - zmin;

    const bool degX = dx <= tolerance;
    const bool degY = dy <= tolerance;
    const bool degZ = dz <= tolerance;

    const int numberOfDegenerateDirections =
        static_cast<int>(degX) +
        static_cast<int>(degY) +
        static_cast<int>(degZ);

    // Three-dimensional bounding box
    if (numberOfDegenerateDirections == 0)
    {
        return BRepPrimAPI_MakeBox(
            gp_Pnt(xmin, ymin, zmin),
            dx, dy, dz).Shape();
    }

    // Two-dimensional bounding box
    if (numberOfDegenerateDirections == 1)
    {
        const double x = 0.5 * (xmin + xmax);
        const double y = 0.5 * (ymin + ymax);
        const double z = 0.5 * (zmin + zmax);

        BRepBuilderAPI_MakePolygon polygon;

        if (degX)
        {
            // Rectangle in the yz-plane
            polygon.Add(gp_Pnt(x, ymin, zmin));
            polygon.Add(gp_Pnt(x, ymax, zmin));
            polygon.Add(gp_Pnt(x, ymax, zmax));
            polygon.Add(gp_Pnt(x, ymin, zmax));
        }
        else if (degY)
        {
            // Rectangle in the xz-plane
            polygon.Add(gp_Pnt(xmin, y, zmin));
            polygon.Add(gp_Pnt(xmin, y, zmax));
            polygon.Add(gp_Pnt(xmax, y, zmax));
            polygon.Add(gp_Pnt(xmax, y, zmin));
        }
        else
        {
            // Rectangle in the xy-plane
            polygon.Add(gp_Pnt(xmin, ymin, z));
            polygon.Add(gp_Pnt(xmax, ymin, z));
            polygon.Add(gp_Pnt(xmax, ymax, z));
            polygon.Add(gp_Pnt(xmin, ymax, z));
        }

        polygon.Close();

        if (!polygon.IsDone())
            throw std::runtime_error("Could not create the rectangular wire.");

        BRepBuilderAPI_MakeFace faceMaker(
            polygon.Wire(),
            Standard_True);

        if (!faceMaker.IsDone())
            throw std::runtime_error("Could not create the rectangular face.");

        return faceMaker.Shape();
    }

    // One-dimensional bounding box
    if (numberOfDegenerateDirections == 2)
    {
        gp_Pnt axisStart;
        gp_Dir axisDirection;
        double length = 0.0;

        const double x = 0.5 * (xmin + xmax);
        const double y = 0.5 * (ymin + ymax);
        const double z = 0.5 * (zmin + zmax);

        if (!degX)
        {
            axisStart = gp_Pnt(xmin, y, z);
            axisDirection = gp_Dir(1.0, 0.0, 0.0);
            length = dx;
        }
        else if (!degY)
        {
            axisStart = gp_Pnt(x, ymin, z);
            axisDirection = gp_Dir(0.0, 1.0, 0.0);
            length = dy;
        }
        else
        {
            axisStart = gp_Pnt(x, y, zmin);
            axisDirection = gp_Dir(0.0, 0.0, 1.0);
            length = dz;
        }

        double radius = lineRadius;

        if (radius <= tolerance)
            radius = std::max(0.005 * length, 10.0 * tolerance);

        return BRepPrimAPI_MakeCylinder(
            gp_Ax2(axisStart, axisDirection),
            radius,
            length).Shape();
    }

    return TopoDS_Shape(); // he bounding box degenerates to a single point.
}

void EntityLumpedFDTDPort::determineLabelOrientation(double xmin, double xmax, double ymin, double ymax, double zmin, double zmax)
{
    EntityPropertiesSelection* normalProperty = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty("Text normal"));
    EntityPropertiesSelection* directionProperty = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty("Text direction"));
    if (!normalProperty || !directionProperty) return;

    textNormal = getVectorFromText(normalProperty->getValue());
    textDirU   = getVectorFromText(directionProperty->getValue());

    textPosition[0] = 0.5 * (xmin + xmax);
    textPosition[1] = 0.5 * (ymin + ymax);
    textPosition[2] = 0.5 * (zmin + zmax);

    double offset = 1e-3 * sqrt((xmax-xmin)*(xmax-xmin) + (ymax - ymin) * (ymax - ymin) + (zmax - zmin) * (zmax - zmin));

    if (normalProperty->getValue() == "-X")
    {
        textPosition[0] = xmin - offset;
    }
    else if(normalProperty->getValue() == "+X")
    {
        textPosition[0] = xmax + offset;
    }
    else if (normalProperty->getValue() == "-Y")
    {
        textPosition[1] = ymin - offset;
    }
    else if (normalProperty->getValue() == "+Y")
    {
        textPosition[1] = ymax + offset;
    }
    else if (normalProperty->getValue() == "-Z")
    {
        textPosition[2] = zmin - offset;
    }
    else if (normalProperty->getValue() == "+Z")
    {
        textPosition[2] = zmax + offset;
    }
    else
    {
        assert(0); // Unknown selection
    }

    textString = getNameOnly();
}

std::vector<double> EntityLumpedFDTDPort::getVectorFromText(const std::string& direction)
{
    if (direction == "+X")
    {
        return std::vector<double>{1.0, 0.0, 0.0};
    }
    else if (direction == "-X")
    {
        return std::vector<double>{-1.0, 0.0, 0.0};
    }
    else if (direction == "+Y")
    {
        return std::vector<double>{0.0, 1.0, 0.0};
    }
    else if (direction == "-Y")
    {
        return std::vector<double>{0.0, -1.0, 0.0};
    }
    else if (direction == "+Z")
    {
        return std::vector<double>{0.0, 0.0, 1.0};
    }
    else if (direction == "-Z")
    {
        return std::vector<double>{0.0, 0.0, -1.0};
    }
    else
    {
        assert(0); // unknown direction
    }

    return std::vector<double>{0.0, 0.0, 0.0};
}

void EntityLumpedFDTDPort::addSpecificMembersForVisualization(ot::JsonDocument& doc)
{
    if (!textString.empty())
    {
        doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_TextString, ot::JsonString(textString, doc.GetAllocator()), doc.GetAllocator());
        doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_TextPosition, ot::JsonArray(textPosition, doc.GetAllocator()), doc.GetAllocator());
        doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_TextNormal, ot::JsonArray(textNormal, doc.GetAllocator()), doc.GetAllocator());
        doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_TextDirU, ot::JsonArray(textDirU, doc.GetAllocator()), doc.GetAllocator());
    }
}

void EntityLumpedFDTDPort::setName(const std::string& _name)
{
    EntityGeometry::setName(_name);

    if (!textString.empty())
    {
        textString = getNameOnly();
        addVisualizationNodes();
    }
}


