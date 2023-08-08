#include "EntityFile.h"
#include <fstream>
#include <iostream>
#include <memory>
#include <OpenTwinCommunication/ActionTypes.h>

EntityFile::EntityFile(ot::UID ID, EntityBase * parent, EntityObserver * obs, ModelState * ms, ClassFactory * factory, const std::string & owner) :
	EntityBase(ID,parent,obs,ms,factory,owner)
{
}

bool EntityFile::getEntityBox(double & xmin, double & xmax, double & ymin, double & ymax, double & zmin, double & zmax)
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
	if (!getName().empty())
	{
		TreeIcon treeIcons;
		treeIcons.size = 32;
		treeIcons.visibleIcon = "TextVisible";
		treeIcons.hiddenIcon = "TextHidden";

		OT_rJSON_createDOC(doc);
		ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_VIEW_AddContainerNode);
		ot::rJSON::add(doc, OT_ACTION_PARAM_UI_TREE_Name, getName());
		ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_EntityID, getEntityID());
		ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ITM_IsEditable, getEditable());

		treeIcons.addToJsonDoc(&doc);

		getObserver()->sendMessageToViewer(doc);
	}
	EntityBase::addVisualizationNodes();
}

void EntityFile::setFileProperties(std::string path, std::string fileName, std::string fileType)
{
	_path = path;
	_fileName = fileName;
	_fileType = fileType;
	setProperties();
}

std::shared_ptr<EntityBinaryData> EntityFile::getData()
{
	EnsureDataIsLoaded();
	return _data;
}

void EntityFile::EnsureDataIsLoaded()
{
	if (_data == nullptr)
	{
		std::map<ot::UID, EntityBase*> entitymap;
		auto entityBase = readEntityFromEntityIDAndVersion(this, _dataUID, _dataVersion, entitymap);
		assert(entityBase != nullptr);
		_data.reset(dynamic_cast<EntityBinaryData*>(entityBase));
	}
}

void EntityFile::setProperties()
{
	auto filePathProperty = new EntityPropertiesString("Path", _path);
	filePathProperty->setReadOnly(true);
	auto fileNameProperty = new EntityPropertiesString("Filename", _fileName);
	fileNameProperty->setReadOnly(true);
	auto fileType = new EntityPropertiesString("FileType", _fileType);
	fileType->setReadOnly(true);
	getProperties().createProperty(filePathProperty, "Selected File");
	getProperties().createProperty(fileNameProperty, "Selected File");
	getProperties().createProperty(fileType, "Selected File");
	
	setSpecializedProperties();
}

void EntityFile::setData(ot::UID dataID, ot::UID dataVersion)
{
	_dataUID = dataID;
	_dataVersion = dataVersion;
}

void EntityFile::AddStorageData(bsoncxx::builder::basic::document & storage)
{
	// We store the parent class information first 
	EntityBase::AddStorageData(storage);

	storage.append(
		bsoncxx::builder::basic::kvp("FilePath", _path),
		bsoncxx::builder::basic::kvp("FileName", _fileName),
		bsoncxx::builder::basic::kvp("FileType", _fileType),
		bsoncxx::builder::basic::kvp("DataUID", _dataUID),
		bsoncxx::builder::basic::kvp("DataVersionID", _dataVersion)
	);
}

void EntityFile::readSpecificDataFromDataBase(bsoncxx::document::view & doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);
	_fileName = doc_view["FileName"].get_utf8().value.to_string();
	_path = doc_view["FilePath"].get_utf8().value.to_string();
	_fileType = doc_view["FileType"].get_utf8().value.to_string();
	_dataUID = doc_view["DataUID"].get_int64();
	_dataVersion = doc_view["DataVersionID"].get_int64();
}
