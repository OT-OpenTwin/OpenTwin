// OpenTwin header
#include "EntityFile.h"
#include "OTCore/JSON.h"
#include "OTCommunication/ActionTypes.h"
#include "VisualisationTypeSerialiser.h"

// std header
#include <fstream>
#include <iostream>
#include <memory>

EntityFile::EntityFile(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms, ClassFactoryHandler* _factory, const std::string& _owner) :
	EntityBase(_ID,_parent,_obs,_ms,_factory,_owner)
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
	TreeIcon treeIcons;
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

std::shared_ptr<EntityBinaryData> EntityFile::getData()
{
	EnsureDataIsLoaded();
	return m_data;
}

void EntityFile::EnsureDataIsLoaded()
{
	if (m_data == nullptr)
	{
		assert(m_dataUID != -1 && m_dataVersion != -1);
		std::map<ot::UID, EntityBase*> entitymap;
		EntityBase* entityBase = readEntityFromEntityIDAndVersion(this, m_dataUID, m_dataVersion, entitymap);
		assert(entityBase != nullptr);
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

void EntityFile::setData(ot::UID _dataID, ot::UID _dataVersion)
{
	m_dataUID = _dataID;
	m_dataVersion = _dataVersion;
	m_data.reset();
	m_data = nullptr;
	setModified();
}

void EntityFile::AddStorageData(bsoncxx::builder::basic::document& _storage)
{
	// We store the parent class information first 
	EntityBase::AddStorageData(_storage);

	_storage.append(
		bsoncxx::builder::basic::kvp("FilePath", m_path),
		bsoncxx::builder::basic::kvp("FileName", m_fileName),
		bsoncxx::builder::basic::kvp("FileType", m_fileType),
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
	m_dataUID = _doc_view["DataUID"].get_int64();
	m_dataVersion = _doc_view["DataVersionID"].get_int64();
}
