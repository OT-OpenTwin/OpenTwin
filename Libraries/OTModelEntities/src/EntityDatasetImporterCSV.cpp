#include "OTModelEntities/EntityDatasetImporterCSV.h"
#include "OTCore/FolderNames.h"
#include "OTCommunication/ActionTypes.h"
#include "OTModelEntities/Properties/PropertyHelper.h"
static EntityFactoryRegistrar<EntityDatasetImporterCSV> registrar(EntityDatasetImporterCSV::className());

std::map<std::string, EntityDatasetImporterCSV::MetadataSelectionFormat> EntityDatasetImporterCSV::m_metadataIdentificationsByName =
{
	{"None",EntityDatasetImporterCSV::MetadataSelectionFormat::None},
	{"Same name as table",EntityDatasetImporterCSV::MetadataSelectionFormat::SameName},
};

std::map<std::string, EntityDatasetImporterCSV::CSVSelectionFormat> EntityDatasetImporterCSV::m_consideredCSVByName =
{
	{"All",EntityDatasetImporterCSV::CSVSelectionFormat::All},
	{"Regex",EntityDatasetImporterCSV::CSVSelectionFormat::Regex}
};

std::map<std::string, EntityDatasetImporterCSV::NamingFormat> EntityDatasetImporterCSV::m_namingFormatByName =
{
	{"Table name", EntityDatasetImporterCSV::NamingFormat::TableName},
	{"Custom", EntityDatasetImporterCSV::NamingFormat::Custom}
};

EntityDatasetImporterCSV::EntityDatasetImporterCSV(ot::UID _ID, EntityBase* _parent, EntityObserver* _obs, ModelState* _ms)
	: EntityBase(_ID,_parent,_obs,_ms)
{
	ot::EntityTreeItem treeItem = getTreeItem();
	treeItem.setVisibleIcon("Default/BatchProcessing");
	treeItem.setHiddenIcon("Default/BatchProcessing");
	this->setDefaultTreeItem(treeItem);
}

void EntityDatasetImporterCSV::addVisualizationNodes(void)
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_OBJ_AddSceneNode, doc.GetAllocator()), doc.GetAllocator());

	doc.AddMember(OT_ACTION_PARAM_TreeItem, ot::JsonObject(this->getTreeItem(), doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_VisualizationTypes, ot::JsonObject(this->getVisualizationTypes(), doc.GetAllocator()), doc.GetAllocator());

	getObserver()->sendMessageToViewer(doc);
}

bool EntityDatasetImporterCSV::updateFromProperties(void)
{
	auto considerCSVProperty = PropertyHelper::getSelectionProperty(this, "CSV file selection");
	auto metadataSelectionProperty = PropertyHelper::getSelectionProperty(this, "Metadata selection");
	auto namingFormat = PropertyHelper::getSelectionProperty(this, "Naming format");
	bool updateGrid = false;
	if (considerCSVProperty->needsUpdate())
	{
		const std::string format = considerCSVProperty->getValue();
		auto consideredCSVByName = m_consideredCSVByName.find(format);
		assert(consideredCSVByName != m_consideredCSVByName.end());
		if (consideredCSVByName->second == CSVSelectionFormat::Regex)
		{
			PropertyHelper::getStringProperty(this, "CSV regex pattern")->setVisible(true);
		}
		else
		{
			PropertyHelper::getStringProperty(this, "CSV regex pattern")->setVisible(false);
		}
		updateGrid = true;
	}

	if (metadataSelectionProperty->needsUpdate())
	{
		const std::string format = metadataSelectionProperty->getValue();
		auto metadataIdentificationsByName= m_metadataIdentificationsByName.find(format);
		assert(metadataIdentificationsByName != m_metadataIdentificationsByName.end());
		if (metadataIdentificationsByName->second == MetadataSelectionFormat::Regex)
		{
			PropertyHelper::getStringProperty(this, "Metadata regex pattern")->setVisible(true);
		}
		else
		{
			PropertyHelper::getStringProperty(this, "Metadata regex pattern")->setVisible(false);
		}
		updateGrid = true;
	}

	if (namingFormat->needsUpdate())
	{
		const std::string format = namingFormat->getValue();
		auto namingFormatByName = m_namingFormatByName.find(format);
		if (namingFormatByName->second == NamingFormat::Custom)
		{
			PropertyHelper::getStringProperty(this, "Name base")->setVisible(true);
		}
		else
		{
			PropertyHelper::getStringProperty(this, "Name base")->setVisible(false);
		}
		updateGrid = true;
	}
	return updateGrid;
}
EntityDatasetImporterCSV::CSVSelectionFormat EntityDatasetImporterCSV::getCSVSelectionFormat()
{
	return CSVSelectionFormat();
}

EntityDatasetImporterCSV::MetadataSelectionFormat EntityDatasetImporterCSV::getMetadataSelectionFormat()
{
	return MetadataSelectionFormat();
}

void EntityDatasetImporterCSV::createProperties(const std::string& _rmdClassificationFolderName, ot::UID _rmdClassificationFolderUID)
{

	EntityPropertiesEntityList::createProperty("Settings", "Classification schema",_rmdClassificationFolderName, _rmdClassificationFolderUID, "", ot::invalidUID, "default", getProperties());

	std::list<std::string> allCSVConsiderations;
	for (auto& entry : m_consideredCSVByName)
	{
		allCSVConsiderations.push_back(entry.first);
	}
	EntityPropertiesSelection::createProperty("Settings", "CSV file selection", allCSVConsiderations, "All", "default", getProperties());
	EntityPropertiesString* csvRegex = EntityPropertiesString::createProperty("Settings", "CSV regex pattern", "", "default", getProperties());
	csvRegex->setVisible(false);

	std::list<std::string> allMetadataIdentifications;
	for (auto& entry : m_metadataIdentificationsByName)
	{
		allMetadataIdentifications.push_back(entry.first);
	}
	EntityPropertiesSelection::createProperty("Settings", "Metadata selection", allMetadataIdentifications, allMetadataIdentifications.front(), "default", getProperties());
	EntityPropertiesString* metadataRegex = EntityPropertiesString::createProperty("Settings", "Metadata regex pattern", "", "default", getProperties());
	metadataRegex->setVisible(false);

	std::list<std::string> allNamingFormats;
	for (auto& entry : m_namingFormatByName)
	{
		allNamingFormats.push_back(entry.first);

	}
	EntityPropertiesSelection::createProperty("Settings", "Naming format", allNamingFormats, allNamingFormats.back(), "default", getProperties());
	EntityPropertiesString* namingBase = EntityPropertiesString::createProperty("Settings", "Name base", "", "default", getProperties());
	namingBase->setVisible(false);
}
