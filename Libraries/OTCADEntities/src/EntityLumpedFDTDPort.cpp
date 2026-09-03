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
}

TopoDS_Shape EntityLumpedFDTDPort::createShape(double lineRadius, double tolerance)
{
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



