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
    EntityPropertiesSelection::createProperty("General", "Up direction", { "-X", "+X", "-Y", "+Y", "-Z", "+Z" }, "+Y", "Microstrip Ports", getProperties())->setToolTip("Specify the direction from the groundplane to the conductor");

    EntityPropertiesString::createProperty("Port range", "Xmin", "0", "Microstrip Ports", getProperties())->setGroupChanges(true);
    EntityPropertiesString::createProperty("Port range", "Xmax", "0", "Microstrip Ports", getProperties())->setGroupChanges(true);
    EntityPropertiesString::createProperty("Port range", "Ymin", "0", "Microstrip Ports", getProperties())->setGroupChanges(true);
    EntityPropertiesString::createProperty("Port range", "Ymax", "0", "Microstrip Ports", getProperties())->setGroupChanges(true);
    EntityPropertiesString::createProperty("Port range", "Zmin", "0", "Microstrip Ports", getProperties())->setGroupChanges(true);
    EntityPropertiesString::createProperty("Port range", "Zmax", "0", "Microstrip Ports", getProperties())->setGroupChanges(true);

	EntityPropertiesDouble::createProperty("Port range", "#Xmin", 0, "Microstrip Ports", getProperties())->setVisible(false);
	EntityPropertiesDouble::createProperty("Port range", "#Xmax", 0, "Microstrip Ports", getProperties())->setVisible(false);
	EntityPropertiesDouble::createProperty("Port range", "#Ymin", 0, "Microstrip Ports", getProperties())->setVisible(false);
	EntityPropertiesDouble::createProperty("Port range", "#Ymax", 0, "Microstrip Ports", getProperties())->setVisible(false);
	EntityPropertiesDouble::createProperty("Port range", "#Zmin", 0, "Microstrip Ports", getProperties())->setVisible(false);
	EntityPropertiesDouble::createProperty("Port range", "#Zmax", 0, "Microstrip Ports", getProperties())->setVisible(false);
}

TopoDS_Shape EntityMicrostripPort::createShape(double lineRadius, double tolerance)
{
    getProperties().forceResetUpdateForAllProperties();

    EntityPropertiesSelection* propagationDirProperty = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty("Propagation direction"));
    EntityPropertiesSelection* upDirProperty = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty("Up direction"));
    assert(propagationDirProperty && upDirProperty);

    EntityPropertiesDouble* xMinProperty = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("#Xmin"));
    EntityPropertiesDouble* xMaxProperty = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("#Xmax"));
    EntityPropertiesDouble* yMinProperty = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("#Ymin"));
    EntityPropertiesDouble* yMaxProperty = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("#Ymax"));
    EntityPropertiesDouble* zMinProperty = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("#Zmin"));
    EntityPropertiesDouble* zMaxProperty = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("#Zmax"));
    assert(xMinProperty && xMaxProperty && yMinProperty && yMaxProperty && zMinProperty && zMaxProperty);

    std::string propagationDirection = propagationDirProperty->getValue();
    std::string upDirection          = upDirProperty->getValue();

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

    determinePortLabel(propagationDirection, upDirection, xmin, xmax, ymin, ymax, zmin, zmax);

    return createMicrostripPortBlock(xmin, xmax, ymin, ymax, zmin, zmax);
}

void EntityMicrostripPort::determinePortLabel(const std::string& propagationDirection,
                                              const std::string &upDirection,
                                              double xmin, double xmax, double ymin, double ymax, double zmin, double zmax)
{
    textPosition = { 0.5 * (xmin + xmax), 0.5 * (ymin + ymax), 0.5 * (zmin + zmax) };

    double offset = 2e-3 * sqrt((xmax-xmin)*(xmax-xmin) + (ymax - ymin) * (ymax - ymin) + (zmax - zmin) * (zmax - zmin));

    if (propagationDirection == "-X")
    {
        textPosition[0] = xmax + offset;
        textNormal = { 1.0, 0.0, 0.0 };
    }
    else if (propagationDirection == "+X")
    {
        textPosition[0] = xmin - offset;
        textNormal = { -1.0, 0.0, 0.0 };
    }
    else if (propagationDirection == "-Y")
    {
        textPosition[1] = ymax + offset;
        textNormal = { 0.0, 1.0, 0.0 };
    }
    else if (propagationDirection == "+Y")
    {
        textPosition[1] = ymin - offset;
        textNormal = { 0.0, -1.0, 0.0 };
    }
    else if (propagationDirection == "-Z")
    {
        textPosition[2] = zmax + offset;
        textNormal = { 0.0, 0.0, 1.0 };
    }
    else if (propagationDirection == "+Z")
    {
        textPosition[2] = zmin - offset;
        textNormal = { 0.0, 0.0, -1.0 };
    }

    std::vector<double> up = { 0.0, 0.0, 0.0 };

    if (upDirection[1] == 'X')
    {
        up[0] = 1.0;
    }
    else if (upDirection[1] == 'Y')
    {
        up[1] = 1.0;
    }
    else if (upDirection[1] == 'Z')
    {
        up[2] = 1.0;
    }

    if (upDirection[0] == '-')
    {
        up = { -up[0], -up[1], -up[2] };
    }

    // U direction: up x textNormal.
    textDirU = {
        up[1] * textNormal[2] - up[2] * textNormal[1],
        up[2] * textNormal[0] - up[0] * textNormal[2],
        up[0] * textNormal[1] - up[1] * textNormal[0]
    };

    textString = getNameOnly();
}

TopoDS_Shape EntityMicrostripPort::createMicrostripPortBlock(double xmin, double xmax, double ymin, double ymax, double zmin, double zmax)
{
    double offset = 1e-3 * sqrt((xmax - xmin) * (xmax - xmin) + (ymax - ymin) * (ymax - ymin) + (zmax - zmin) * (zmax - zmin));

    xmin -= offset;
    xmax += offset;
    ymin -= offset;
    ymax += offset;
    zmin -= offset;
    zmax += offset;

    BRepPrimAPI_MakeBox box(gp_Pnt(xmin, ymin, zmin), gp_Pnt(xmax, ymax, zmax));

    try
    {
        box.Build();

        if (!box.IsDone()) return TopoDS_Shape();
    }
    catch (const Standard_Failure&) 
    {
        return TopoDS_Shape();
    }

    return box.Shape();
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


