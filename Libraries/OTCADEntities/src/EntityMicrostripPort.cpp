// @otlicense
// File: EntityMicrostripPort.cpp
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
#include "OTCADEntities/EntityMicrostripPort.h"

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

static EntityFactoryRegistrar<EntityMicrostripPort> registrar(EntityMicrostripPort::className());

EntityMicrostripPort::EntityMicrostripPort(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms) :
	EntityGeometry(ID, parent, obs, ms)
{
	createProperties();
}

EntityMicrostripPort::~EntityMicrostripPort()
{
}

void EntityMicrostripPort::addStorageData(bsoncxx::builder::basic::document& storage)
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

void EntityMicrostripPort::readSpecificDataFromDataBase(const bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
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

void EntityMicrostripPort::clearText(void)
{
    textString.clear();
    textPosition = { 0.0, 0.0, 0.0 };
    textNormal = { 0.0, 0.0, 0.0 };
    textDirU = { 0.0, 0.0, 0.0 };
}

void EntityMicrostripPort::createProperties()
{
	EntityPropertiesColor::createProperty("General", "Color", { 255, 171, 0 }, "Microstrip Ports", getProperties());
    EntityPropertiesSelection::createProperty("General", "Propagation direction", { "-X", "+X", "-Y", "+Y", "-Z", "+Z" }, "+Z", "Microstrip Ports", getProperties());
    EntityPropertiesSelection::createProperty("General", "Current direction", { "-X", "+X", "-Y", "+Y", "-Z", "+Z" }, "+Y", "Microstrip Ports", getProperties());

    EntityPropertiesDouble::createProperty("Port range", "Position X", 0.0, "Microstrip Ports", getProperties())->setGroupChanges(true);;
    EntityPropertiesDouble::createProperty("Port range", "Position Y", 0.0, "Microstrip Ports", getProperties())->setGroupChanges(true);;
    EntityPropertiesDouble::createProperty("Port range", "Position Z", 0.0, "Microstrip Ports", getProperties())->setGroupChanges(true);;

	EntityPropertiesDouble::createProperty("Port range", "Xmin", 0, "Microstrip Ports", getProperties())->setGroupChanges(true);
	EntityPropertiesDouble::createProperty("Port range", "Xmax", 0, "Microstrip Ports", getProperties())->setGroupChanges(true);
	EntityPropertiesDouble::createProperty("Port range", "Ymin", 0, "Microstrip Ports", getProperties())->setGroupChanges(true);
	EntityPropertiesDouble::createProperty("Port range", "Ymax", 0, "Microstrip Ports", getProperties())->setGroupChanges(true);
	EntityPropertiesDouble::createProperty("Port range", "Zmin", 0, "Microstrip Ports", getProperties())->setGroupChanges(true);
	EntityPropertiesDouble::createProperty("Port range", "Zmax", 0, "Microstrip Ports", getProperties())->setGroupChanges(true);
}

TopoDS_Shape EntityMicrostripPort::createShape(double lineRadius, double tolerance)
{
    getProperties().forceResetUpdateForAllProperties();

    EntityPropertiesSelection* propagationDirProperty = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty("Propagation direction"));
    EntityPropertiesSelection* currentDirProperty = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty("Current direction"));
    assert(propagationDirProperty && currentDirProperty);

    EntityPropertiesDouble* xPosProperty = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("Position X"));
    EntityPropertiesDouble* yPosProperty = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("Position Y"));
    EntityPropertiesDouble* zPosProperty = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("Position Z"));
    assert(xPosProperty && yPosProperty && zPosProperty);

    EntityPropertiesDouble* xMinProperty = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("Xmin"));
    EntityPropertiesDouble* xMaxProperty = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("Xmax"));
    EntityPropertiesDouble* yMinProperty = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("Ymin"));
    EntityPropertiesDouble* yMaxProperty = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("Ymax"));
    EntityPropertiesDouble* zMinProperty = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("Zmin"));
    EntityPropertiesDouble* zMaxProperty = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("Zmax"));
    assert(xMinProperty && xMaxProperty && yMinProperty && yMaxProperty && zMinProperty && zMaxProperty);

    std::string propagationDirection = propagationDirProperty->getValue();
    std::string currentDirection     = currentDirProperty->getValue();

    double xpos = xPosProperty->getValue();
    double ypos = yPosProperty->getValue();
    double zpos = zPosProperty->getValue();

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

    determinePortLabel(propagationDirection, currentDirection, xmin, xmax, ymin, ymax, zmin, zmax, xpos, ypos, zpos);

    return createMicrostripPortFace(propagationDirection, xmin, xmax, ymin, ymax, zmin, zmax, xpos, ypos, zpos);
}

void EntityMicrostripPort::determinePortLabel(const std::string& propagationDirection,
                                              const std::string &currentDirection,
                                              double xmin, double xmax, double ymin, double ymax, double zmin, double zmax,
                                              double xpos, double ypos, double zpos)
{
    const auto parseDirection =
        [](const std::string& direction) -> std::array<double, 3>
        {
            if (direction.size() != 2 ||
                (direction[0] != '+' && direction[0] != '-'))
            {
                assert(0); // Invalid direction
            }

            const double sign = direction[0] == '+' ? 1.0 : -1.0;
            const char axis = static_cast<char>(
                std::toupper(
                    static_cast<unsigned char>(direction[1])));

            switch (axis)
            {
            case 'X':
                return { sign, 0.0, 0.0 };

            case 'Y':
                return { 0.0, sign, 0.0 };

            case 'Z':
                return { 0.0, 0.0, sign };

            default:
                assert(0); // Invalid direction
                return { 0.0, 0.0, 0.0 };
            }
        };

    const std::array<double, 3> propagation = parseDirection(propagationDirection);

    const std::array<double, 3> current = parseDirection(currentDirection);

    std::array<double, 3> center = {
        xmin + 0.5 * (xmax - xmin),
        ymin + 0.5 * (ymax - ymin),
        zmin + 0.5 * (zmax - zmin)
    };

    const double dx = xmax - xmin;
    const double dy = ymax - ymin;
    const double dz = zmax - zmin;

    double portDiagonal = 0.0;

    if (propagation[0] != 0.0)
    {
        // Port plane is perpendicular to the X axis.
        center[0] = xpos;
        portDiagonal = std::sqrt(dy * dy + dz * dz);
    }
    else if (propagation[1] != 0.0)
    {
        // Port plane is perpendicular to the Y axis.
        center[1] = ypos;
        portDiagonal = std::sqrt(dx * dx + dz * dz);
    }
    else
    {
        // Port plane is perpendicular to the Z axis.
        center[2] = zpos;
        portDiagonal = std::sqrt(dx * dx + dy * dy);
    }

    const double offset = 1e-3 * portDiagonal;

    // Move the text against the propagation direction.
    textPosition = {
        center[0] - offset * propagation[0],
        center[1] - offset * propagation[1],
        center[2] - offset * propagation[2]
    };

    // The text normal points against the propagation direction.
    textNormal = {
        -propagation[0],
        -propagation[1],
        -propagation[2]
    };

    // U direction: propagationDirection x currentDirection.
    textDirU = {
        propagation[1] * current[2] -
            propagation[2] * current[1],

        propagation[2] * current[0] -
            propagation[0] * current[2],

        propagation[0] * current[1] -
            propagation[1] * current[0]
    };

    textString = getNameOnly();
}

TopoDS_Shape EntityMicrostripPort::createMicrostripPortFace(const std::string& propagationDirection, 
                                                            double xmin, double xmax, double ymin, double ymax, double zmin, double zmax, 
                                                            double xpos, double ypos, double zpos)
{
    if (propagationDirection.size() != 2 ||
        (propagationDirection[0] != '+' &&
            propagationDirection[0] != '-'))
    {
        return TopoDS_Shape();
    }

    const bool positive = propagationDirection[0] == '+';
    const char axis = static_cast<char>(
        std::toupper(static_cast<unsigned char>(
            propagationDirection[1])));

    std::array<gp_Pnt, 4> points;

    switch (axis)
    {
    case 'X':
        if (ymax <= ymin || zmax <= zmin)
            return TopoDS_Shape();

        // Point order produces a normal in +X direction.
        points = {
            gp_Pnt(xpos, ymin, zmin),
            gp_Pnt(xpos, ymax, zmin),
            gp_Pnt(xpos, ymax, zmax),
            gp_Pnt(xpos, ymin, zmax)
        };
        break;

    case 'Y':
        if (xmax <= xmin || zmax <= zmin)
            return TopoDS_Shape();

        // Point order produces a normal in +Y direction.
        points = {
            gp_Pnt(xmin, ypos, zmin),
            gp_Pnt(xmin, ypos, zmax),
            gp_Pnt(xmax, ypos, zmax),
            gp_Pnt(xmax, ypos, zmin)
        };
        break;

    case 'Z':
        if (xmax <= xmin || ymax <= ymin)
            return TopoDS_Shape();

        // Point order produces a normal in +Z direction.
        points = {
            gp_Pnt(xmin, ymin, zpos),
            gp_Pnt(xmax, ymin, zpos),
            gp_Pnt(xmax, ymax, zpos),
            gp_Pnt(xmin, ymax, zpos)
        };
        break;

    default:
        return TopoDS_Shape();
    }

    // Reverse the face orientation for -X, -Y or -Z.
    if (!positive)
        std::reverse(points.begin(), points.end());

    BRepBuilderAPI_MakePolygon polygon;

    for (const gp_Pnt& point : points)
        polygon.Add(point);

    polygon.Close();

    if (!polygon.IsDone())
        return TopoDS_Shape();

    BRepBuilderAPI_MakeFace faceMaker(polygon.Wire(), true);

    if (!faceMaker.IsDone())
        return TopoDS_Shape();

    return faceMaker.Shape();
}

std::vector<double> EntityMicrostripPort::getVectorFromText(const std::string& direction)
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

void EntityMicrostripPort::addSpecificMembersForVisualization(ot::JsonDocument& doc)
{
    if (!textString.empty())
    {
        doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_TextString, ot::JsonString(textString, doc.GetAllocator()), doc.GetAllocator());
        doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_TextPosition, ot::JsonArray(textPosition, doc.GetAllocator()), doc.GetAllocator());
        doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_TextNormal, ot::JsonArray(textNormal, doc.GetAllocator()), doc.GetAllocator());
        doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_TextDirU, ot::JsonArray(textDirU, doc.GetAllocator()), doc.GetAllocator());
    }
}

void EntityMicrostripPort::setName(const std::string& _name)
{
    EntityGeometry::setName(_name);

    if (!textString.empty())
    {
        textString = getNameOnly();
        addVisualizationNodes();
    }
}


