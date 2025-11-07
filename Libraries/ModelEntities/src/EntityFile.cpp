// @otlicense
// File: EntityFile.cpp
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
#include "EntityFile.h"
#include "OTCore/JSON.h"
#include "OTGui/FileExtension.h"
#include "OTCommunication/ActionTypes.h"
#include "VisualisationTypeSerialiser.h"

// std header
#include <fstream>
#include <iostream>
#include <memory>

static EntityFactoryRegistrar<EntityFile> registrar(EntityFile::className());

EntityFile::EntityFile(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms) :
	EntityBase(_ID, _parent, _obs, _ms), m_fileFilter(ot::FileExtension::toFilterString(ot::FileExtension::AllFiles)),
	m_dataUID(ot::invalidUID), m_dataVersion(ot::invalidUID)
{
}

bool EntityFile::getEntityBox(double & _xmin, double & _xmax, double & _ymin, double & _ymax, double & _zmin, double & _zmax)
{
	return false;
}

bool EntityFile::updateFromProperties(void)
{
	// Now we need to update the entity after a property change
	assert(getProperties().anyPropertyNeedsUpdate());

	// Since there is a change now, we need to set the modified flag
	setModified();

	// Here we need to update the plot (send a message to the visualization service)
	getProperties().forceResetUpdateForAllProperties();
	return false;
}

void EntityFile::addVisualizationNodes(void)
{
	OldTreeIcon treeIcons;
	treeIcons.size = 32;
	treeIcons.visibleIcon = "TextVisible";
	treeIcons.hiddenIcon = "TextHidden";

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_AddSceneNode, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_TREE_Name, ot::JsonString(this->getName(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, this->getEntityID(), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_IsEditable, this->getEditable(), doc.GetAllocator());

	VisualisationTypeSerialiser visualisationTypeSerialiser;
	visualisationTypeSerialiser(this, doc, doc.GetAllocator());

	treeIcons.addToJsonDoc(doc);

	getObserver()->sendMessageToViewer(doc);
}

void EntityFile::setFileProperties(const std::string& _path, const std::string& _fileName, const std::string& _fileType)
{
	m_path = _path;
	m_fileName = _fileName;
	m_fileType = _fileType;
	setProperties();
}

std::shared_ptr<EntityBinaryData> EntityFile::getDataEntity()
{
	ensureDataIsLoaded();
	return m_data;
}

void EntityFile::ensureDataIsLoaded()
{
	if (m_data == nullptr)
	{
		assert(m_dataUID != -1 && m_dataVersion != -1);
		std::map<ot::UID, EntityBase*> entitymap;
		EntityBase* entityBase = readEntityFromEntityIDAndVersion(this, m_dataUID, m_dataVersion, entitymap);
		
		if (!entityBase) {
			throw ot::Exception::ObjectNotFound("EntityFile::ensureDataIsLoaded: Unable to load data entity { \"ID\": " + std::to_string(m_dataUID) + ", \"Version\": " + std::to_string(m_dataVersion) + " }");
		}

		entityBase->setObserver(nullptr);
		m_data.reset(dynamic_cast<EntityBinaryData*>(entityBase));
		entityBase = nullptr;
	}
}

void EntityFile::setProperties()
{
	auto filePathProperty = new EntityPropertiesString("Path", m_path);
	filePathProperty->setReadOnly(true);
	auto fileNameProperty = new EntityPropertiesString("Filename", m_fileName);
	fileNameProperty->setReadOnly(true);
	auto fileType = new EntityPropertiesString("FileType", m_fileType);
	fileType->setReadOnly(true);
	getProperties().createProperty(filePathProperty, "Selected File");
	getProperties().createProperty(fileNameProperty, "Selected File");
	getProperties().createProperty(fileType, "Selected File");
	
	setSpecializedProperties();
}

void EntityFile::setDataEntity(ot::UID _dataID, ot::UID _dataVersion)
{
	m_dataUID = _dataID;
	m_dataVersion = _dataVersion;
	m_data.reset();
	m_data = nullptr;
	setModified();
}

void EntityFile::addStorageData(bsoncxx::builder::basic::document& _storage)
{
	// We store the parent class information first 
	EntityBase::addStorageData(_storage);

	_storage.append(
		bsoncxx::builder::basic::kvp("FilePath", m_path),
		bsoncxx::builder::basic::kvp("FileName", m_fileName),
		bsoncxx::builder::basic::kvp("FileType", m_fileType),
		bsoncxx::builder::basic::kvp("FileFilter", m_fileFilter),
		bsoncxx::builder::basic::kvp("DataUID", m_dataUID),
		bsoncxx::builder::basic::kvp("DataVersionID", m_dataVersion)
	);
}

void EntityFile::readSpecificDataFromDataBase(bsoncxx::document::view & _doc_view, std::map<ot::UID, EntityBase*>& _entityMap)
{
	EntityBase::readSpecificDataFromDataBase(_doc_view, _entityMap);
	m_fileName = std::string(_doc_view["FileName"].get_utf8().value.data());
	m_path = std::string(_doc_view["FilePath"].get_utf8().value.data());
	m_fileType = std::string(_doc_view["FileType"].get_utf8().value.data());
	if (_doc_view.find("FileFilter") != _doc_view.end()) { m_fileFilter = _doc_view["FileFilter"].get_utf8().value.data(); }
	m_dataUID = _doc_view["DataUID"].get_int64();
	m_dataVersion = _doc_view["DataVersionID"].get_int64();
}
