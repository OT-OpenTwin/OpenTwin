// @otlicense
// File: PropertyBundlePlane.cpp
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

#include "..\include\PropertyBundlePlane.h"

void PropertyBundlePlane::setProperties(EntityBase * _thisObject)
{
	EntityPropertiesSelection::createProperty("Plane", m_properties.GetPropertyNameNormal(), 
		{ 
			m_properties.GetPropertyValueNormalX(), 
			m_properties.GetPropertyValueNormalY(), 
			m_properties.GetPropertyValueNormalZ(), 
			m_properties.GetPropertyValueNormalFree() 
		}, m_properties.GetPropertyValueNormalZ(), "2D Visualization",_thisObject->getProperties());
	EntityPropertiesDouble::createProperty("Plane", m_properties.GetPropertyNameNormalCooX(), 0.0, "2D Visualization", _thisObject->getProperties());
	EntityPropertiesDouble::createProperty("Plane", m_properties.GetPropertyNameNormalCooY(), 0.0, "2D Visualization", _thisObject->getProperties());
	EntityPropertiesDouble::createProperty("Plane", m_properties.GetPropertyNameNormalCooZ(), 1.0, "2D Visualization", _thisObject->getProperties());
	EntityPropertiesDouble::createProperty("Plane", m_properties.GetPropertyNameCenterX(), 0.0, "2D Visualization", _thisObject->getProperties());
	EntityPropertiesDouble::createProperty("Plane", m_properties.GetPropertyNameCenterY(), 0.0, "2D Visualization", _thisObject->getProperties());
	EntityPropertiesDouble::createProperty("Plane", m_properties.GetPropertyNameCenterZ(), 0.0, "2D Visualization", _thisObject->getProperties());
}

bool PropertyBundlePlane::updatePropertyVisibility(EntityBase * _thisObject)
{
	bool updatePropertiesGrid = false;

	EntityPropertiesSelection *normalDirection = dynamic_cast<EntityPropertiesSelection*>(_thisObject->getProperties().getProperty(m_properties.GetPropertyNameNormal()));

	if (!normalDirection->getVisible()) updatePropertiesGrid = true;
	normalDirection->setVisible(true);

	EntityPropertiesDouble *normalX = dynamic_cast<EntityPropertiesDouble*>(_thisObject->getProperties().getProperty(m_properties.GetPropertyNameNormalCooX()));
	EntityPropertiesDouble *normalY = dynamic_cast<EntityPropertiesDouble*>(_thisObject->getProperties().getProperty(m_properties.GetPropertyNameNormalCooY()));
	EntityPropertiesDouble *normalZ = dynamic_cast<EntityPropertiesDouble*>(_thisObject->getProperties().getProperty(m_properties.GetPropertyNameNormalCooZ()));

	EntityPropertiesDouble *centerX = dynamic_cast<EntityPropertiesDouble*>(_thisObject->getProperties().getProperty(m_properties.GetPropertyNameCenterX()));
	EntityPropertiesDouble *centerY = dynamic_cast<EntityPropertiesDouble*>(_thisObject->getProperties().getProperty(m_properties.GetPropertyNameCenterY()));
	EntityPropertiesDouble *centerZ = dynamic_cast<EntityPropertiesDouble*>(_thisObject->getProperties().getProperty(m_properties.GetPropertyNameCenterZ()));

	PlaneProperties::NormalDescription normal = m_properties.GetNormalDescription(normalDirection->getValue());
	assert(normal != PlaneProperties::NormalDescription::UNKNOWN);

	if (normal == PlaneProperties::Free)
	{
		if (!normalX->getVisible()) updatePropertiesGrid = true;
		if (!normalY->getVisible()) updatePropertiesGrid = true;
		if (!normalZ->getVisible()) updatePropertiesGrid = true;

		normalX->setVisible(true);
		normalY->setVisible(true);
		normalZ->setVisible(true);
	}
	else
	{
		if (normalX->getVisible()) updatePropertiesGrid = true;
		if (normalY->getVisible()) updatePropertiesGrid = true;
		if (normalZ->getVisible()) updatePropertiesGrid = true;

		normalX->setVisible(false);
		normalY->setVisible(false);
		normalZ->setVisible(false);
	}

	bool centerVisibleX = normal == PlaneProperties::Free || normal == PlaneProperties::X ;
	bool centerVisibleY = normal == PlaneProperties::Free || normal == PlaneProperties::Y;
	bool centerVisibleZ = normal == PlaneProperties::Free || normal == PlaneProperties::Z;

	if (centerVisibleX != centerX->getVisible()) updatePropertiesGrid = true;
	if (centerVisibleY != centerY->getVisible()) updatePropertiesGrid = true;
	if (centerVisibleZ != centerZ->getVisible()) updatePropertiesGrid = true;

	centerX->setVisible(centerVisibleX);
	centerY->setVisible(centerVisibleY);
	centerZ->setVisible(centerVisibleZ);

	return updatePropertiesGrid;
}

bool PropertyBundlePlane::hidePlane(EntityBase* _thisObject)
{
	bool updatePropertiesGrid = false;

	EntityPropertiesSelection* normalDirection = dynamic_cast<EntityPropertiesSelection*>(_thisObject->getProperties().getProperty(m_properties.GetPropertyNameNormal()));

	EntityPropertiesDouble* normalX = dynamic_cast<EntityPropertiesDouble*>(_thisObject->getProperties().getProperty(m_properties.GetPropertyNameNormalCooX()));
	EntityPropertiesDouble* normalY = dynamic_cast<EntityPropertiesDouble*>(_thisObject->getProperties().getProperty(m_properties.GetPropertyNameNormalCooY()));
	EntityPropertiesDouble* normalZ = dynamic_cast<EntityPropertiesDouble*>(_thisObject->getProperties().getProperty(m_properties.GetPropertyNameNormalCooZ()));

	EntityPropertiesDouble* centerX = dynamic_cast<EntityPropertiesDouble*>(_thisObject->getProperties().getProperty(m_properties.GetPropertyNameCenterX()));
	EntityPropertiesDouble* centerY = dynamic_cast<EntityPropertiesDouble*>(_thisObject->getProperties().getProperty(m_properties.GetPropertyNameCenterY()));
	EntityPropertiesDouble* centerZ = dynamic_cast<EntityPropertiesDouble*>(_thisObject->getProperties().getProperty(m_properties.GetPropertyNameCenterZ()));

	if (normalDirection->getVisible()) updatePropertiesGrid = true;

	if (normalX->getVisible()) updatePropertiesGrid = true;
	if (normalY->getVisible()) updatePropertiesGrid = true;
	if (normalZ->getVisible()) updatePropertiesGrid = true;
	
	if (centerX->getVisible()) updatePropertiesGrid = true;
	if (centerY->getVisible()) updatePropertiesGrid = true;
	if (centerZ->getVisible()) updatePropertiesGrid = true;

	normalDirection->setVisible(false);

	normalX->setVisible(false);
	normalY->setVisible(false);
	normalZ->setVisible(false);

	centerX->setVisible(false);
	centerY->setVisible(false);
	centerZ->setVisible(false);

	return updatePropertiesGrid;
}
