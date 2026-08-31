// @otlicense
// File: EntityWaveguidePort.cpp
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
#include "OTModelEntities/DataBase.h"
#include "OTModelEntities/EntityFacetData.h"
#include "OTCADEntities/EntityBrep.h"
#include "OTCADEntities/EntityGeometry.h"
#include "OTCADEntities/GeometryOperations.h"
#include "OTCADEntities/EntityWaveguidePort.h"

static EntityFactoryRegistrar<EntityWaveguidePort> registrar(EntityWaveguidePort::className());

EntityWaveguidePort::EntityWaveguidePort() : EntityFaceAnnotation(0, nullptr, nullptr, nullptr)
{
	createProperties();
}

EntityWaveguidePort::EntityWaveguidePort(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms) :
	EntityFaceAnnotation(ID, parent, obs, ms)
{
	createProperties();
}

void EntityWaveguidePort::createProperties()
{
	ot::EntityTreeItem treeItem = getTreeItem();
	treeItem.setVisibleIcon("Default/FaceAnnotationVisible");
	treeItem.setHiddenIcon("Default/FaceAnnotationHidden");
	this->setDefaultTreeItem(treeItem);

	EntityPropertiesSelection::createProperty("Port", "Port type", { "Rectangular" }, "Rectangular", "WaveguidePort", getProperties());
	EntityPropertiesSelection::createProperty("Port", "Mode", { "TE10", "TE01", "TE20", "TE21", "TE22", "TE30", "TE31", "TE32", "TE33" }, "TE10", "WaveguidePort", getProperties());
}

EntityWaveguidePort::~EntityWaveguidePort()
{
}

bool EntityWaveguidePort::updateFromProperties(void)
{
	// Now we need to update the entity after a property change
	assert(getProperties().anyPropertyNeedsUpdate());

	getProperties().forceResetUpdateForAllProperties();

	// The geometry will be updated as special entity update in the modeler.

	return false; // No property grid update necessary
}

void EntityWaveguidePort::addStorageData(bsoncxx::builder::basic::document &storage)
{
	// We store the parent class information first 
	EntityFaceAnnotation::addStorageData(storage);
}

void EntityWaveguidePort::storeToDataBase(void)
{
	EntityFaceAnnotation::storeToDataBase();
}

void EntityWaveguidePort::readSpecificDataFromDataBase(const bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap)
{
	// We read the parent class information first 
	EntityFaceAnnotation::readSpecificDataFromDataBase(doc_view, entityMap);

	resetModified();
}

void EntityWaveguidePort::postGeometryUpdates(void)
{
	clearText();
	if (facets == nullptr) return;

	// We have just updated the facets. Now we can position our label in the middle of the facets
	// Determine the port normal and the largest port extension

	double nx = 0.0, ny = 0.0, nz = 0.0;
	BoundingBox box;

	for (auto& node : facets->getNodeVector())
	{
		box.extend(node.getCoord(0), node.getCoord(1), node.getCoord(2));
		nx += node.getNormal(0);
		ny += node.getNormal(1);
		nz += node.getNormal(2);
	}

	nx /= 1.0 * facets->getNodeVector().size();
	ny /= 1.0 * facets->getNodeVector().size();
	nz /= 1.0 * facets->getNodeVector().size();

	double cx = 0.5 * (box.getXmax() + box.getXmin());
	double cy = 0.5 * (box.getYmax() + box.getYmin());
	double cz = 0.5 * (box.getZmax() + box.getZmin());

	double dx = box.getXmax() - box.getXmin();
	double dy = box.getYmax() - box.getYmin();
	double dz = box.getZmax() - box.getZmin();

	double tolerance = 1e-4;

	textPosition = { cx, cy, cz };
	textNormal = { nx, ny, nz };

	double uComponent = 1.0;

	if (nx < -tolerance || ny < -tolerance || nz < -tolerance)
	{
		// The text direction needs to be inverted
		uComponent = -1.0;
	}

	setTextStringFromName();

	if (fabs(ny) < tolerance && fabs(nz) < tolerance && fabs(fabs(nx) - 1.0) < tolerance)
	{
		// Port normal +/- x direction (u = y, v = z)
		if (dy > dz)
		{
			textDirU = { 0.0, uComponent, 0.0 };
		}
		else
		{
			textDirU = { 0.0, 0.0, -uComponent };
		}
	}
	else if (fabs(nx) < tolerance && fabs(nz) < tolerance && fabs(fabs(ny) - 1.0) < tolerance)
	{
		// Port normal +/- y direction (u = z, v = x)
		if (dz > dx)
		{
			textDirU = { 0.0, 0.0, uComponent };
		}
		else
		{
			textDirU = { -uComponent, 0.0, 0.0 };
		}
	}
	else if (fabs(nx) < tolerance && fabs(ny) < tolerance && fabs(fabs(nz) - 1.0) < tolerance)
	{
		// Port normal +/- z direction (u = x, v = y)
		if (dx > dy)
		{
			textDirU = { uComponent, 0.0, 0.0 };
		}
		else
		{
			textDirU = { 0.0, -uComponent, 0.0 };
		}
	}
	else
	{
		// The port is not aligned with the coordinate axes
		clearText();
	}
}

void EntityWaveguidePort::setTextStringFromName()
{
	std::string name = getName();

	const std::size_t pos = name.find_last_of('/');

	textString = (pos == std::string::npos) ? name : name.substr(pos + 1);
}

void EntityWaveguidePort::setName(const std::string& _name)
{
	EntityFaceAnnotation::setName(_name);
	
	if (!textString.empty())
	{
		setTextStringFromName();
		updateVisualization(false);
	}
}

