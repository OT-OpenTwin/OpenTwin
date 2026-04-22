// @otlicense
// File: EntitySmartPart.cpp
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
#include "OTCADEntities/EntitySmartPart.h"

static EntityFactoryRegistrar<EntitySmartPart> registrar(EntitySmartPart::className());

EntitySmartPart::EntitySmartPart(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms) :
	EntityGeometry(ID, parent, obs, ms)
{

}

EntitySmartPart::~EntitySmartPart()
{
	if (m_data != nullptr)
	{
		delete m_data;
		m_data = nullptr;
	}
}

EntityBinaryData* EntitySmartPart::getData(void)
{
	ensureDataLoaded();
	assert(m_data != nullptr);

	return m_data;
}

void EntitySmartPart::ensureDataLoaded(void)
{
	if (m_data == nullptr)
	{
		if (m_dataStorageId == 0)
		{
			m_data = new EntityBinaryData(getUidGenerator()->getUID(), this, getObserver(), getModelState());
		}
		else
		{
			std::map<ot::UID, EntityBase*> entityMap;
			m_data = dynamic_cast<EntityBinaryData*>(readEntityFromEntityIDAndVersion(this, m_dataStorageId, m_dataStorageVersion, entityMap));
		}

		assert(m_data != nullptr);
	}
}

void EntitySmartPart::storeToDataBase(void)
{
	if (m_data != nullptr)
	{
		storeData();
	}

	EntityGeometry::storeToDataBase();
}

void EntitySmartPart::storeData(void)
{
	if (m_data == nullptr) return;
	assert(m_data != nullptr);

	m_data->storeToDataBase();

	if (m_dataStorageId != m_data->getEntityID() || m_dataStorageVersion != m_data->getEntityStorageVersion())
	{
		setModified();
	}

	m_dataStorageId = m_data->getEntityID();
	m_dataStorageVersion = m_data->getEntityStorageVersion();
}

void EntitySmartPart::addStorageData(bsoncxx::builder::basic::document& storage)
{
	EntityGeometry::addStorageData(storage);
	// Now we store the particular information about the current object
	storage.append(
		bsoncxx::builder::basic::kvp("DataID", static_cast<int64_t>(m_dataStorageId)),
		bsoncxx::builder::basic::kvp("DataVersion", static_cast<int64_t>(m_dataStorageVersion))
	);
}

void EntitySmartPart::readSpecificDataFromDataBase(const bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	// We read the parent class information first 
	EntityGeometry::readSpecificDataFromDataBase(doc_view, entityMap);

	// Here we can load any special information about the entity
	m_dataStorageId = doc_view["DataID"].get_int64();
	m_dataStorageVersion = doc_view["DataVersion"].get_int64();

	resetModified();
}

void EntitySmartPart::removeChild(EntityBase* child)
{
	if (child == m_data)
	{
		m_data = nullptr;
	}
	else
	{
		EntityGeometry::removeChild(child);
	}
}

void EntitySmartPart::createProperties(int colorR, int colorG, int colorB, const std::string& geometryFolder, ot::UID geometryFolderID, const std::string& materialsFolder, ot::UID materialsFolderID)
{
	EntityPropertiesString* typeProp = new EntityPropertiesString("shapeType", "SMART_PART");
	typeProp->setVisible(false);
	getProperties().createProperty(typeProp, "Internal");

	createGroupPropertiesOnly(geometryFolder, geometryFolderID);
	createMaterialPropertiesOnly(colorR, colorG, colorB, materialsFolder, materialsFolderID);
	
	createPartProperties();
	
	createNonMaterialProperties();
}

void EntitySmartPart::addPartProperty(const std::string &name, const std::string &value)
{
	EntityPropertiesString* stringProp = new EntityPropertiesString(name, value);
	stringProp->setGroupChanges(true);

	double doubleValue = evaluateExpressionDouble(value);

	// Create the double property which holds the current value of the expression
	EntityPropertiesDouble* doubleProp = new EntityPropertiesDouble("#" + name, doubleValue);
	doubleProp->setVisible(false);

	getProperties().createProperty(stringProp, "Shape properties");
	getProperties().createProperty(doubleProp, "Shape properties");
}
