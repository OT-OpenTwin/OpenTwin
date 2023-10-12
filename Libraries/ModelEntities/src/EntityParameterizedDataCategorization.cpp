#include "EntityParameterizedDataCategorization.h"

#include <OpenTwinCommunication/ActionTypes.h>

EntityParameterizedDataCategorization::EntityParameterizedDataCategorization(ot::UID ID, EntityBase * parent, EntityObserver * obs, ModelState * ms, ClassFactoryHandler* factory, const std::string & owner)
	:EntityContainer(ID, parent, obs,ms,factory,owner)
{
}

void EntityParameterizedDataCategorization::addVisualizationNodes()
{
	if (!getName().empty())
	{
		TreeIcon treeIcons;
		treeIcons.size = 32;
		treeIcons.visibleIcon = "Categorization";
		treeIcons.hiddenIcon = "Categorization";

		OT_rJSON_createDOC(doc);
		ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_VIEW_AddContainerNode);
		ot::rJSON::add(doc, OT_ACTION_PARAM_UI_TREE_Name, getName());
		ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_EntityID, getEntityID());
		ot::rJSON::add(doc, OT_ACTION_PARAM_MODEL_ITM_IsEditable, getEditable());

		treeIcons.addToJsonDoc(&doc);

		getObserver()->sendMessageToViewer(doc);
	}

	for (auto child : getChildrenList())
	{
		child->addVisualizationNodes();
	}

	EntityBase::addVisualizationNodes();
}

void EntityParameterizedDataCategorization::CreateProperties(DataCategorie categorie)
{
	std::string returnVal;
	_dataCategorieStringMapping.find(categorie) == _dataCategorieStringMapping.end() ? returnVal = "" : returnVal = _dataCategorieStringMapping.at(categorie);
	if (returnVal != "")
	{
		_selectedCategory = categorie;
		//Should be eventually selectable/ changeable.
		//EntityPropertiesSelection::createProperty(_dataCategorieGroup, _dataCategorieGroup, { _dataCategorieRMD, _dataCategorieMSMD, _dataCategorieParam, _dataCategorieQuant }, returnVal, _defaultCategory, getProperties());
		EntityPropertiesString::createProperty(_dataCategorieGroup, _dataCategorieGroup, returnVal, _defaultCategory, getProperties());
	}
}

void EntityParameterizedDataCategorization::AddStorageData(bsoncxx::builder::basic::document & storage)
{
	if (_selectedCategory == DataCategorie::UNKNOWN)
	{
		throw std::runtime_error("Data category in EntityParameterizedDataCategorization was not set.");
	}
	std::string returnVal;
	_dataCategorieStringMapping.find(_selectedCategory) == _dataCategorieStringMapping.end() ? returnVal = "" : returnVal = _dataCategorieStringMapping.at(_selectedCategory);
	if (returnVal != "")
	{
		EntityContainer::AddStorageData(storage);
		storage.append(
			bsoncxx::builder::basic::kvp("Category", returnVal)
		);
	}
}

void EntityParameterizedDataCategorization::readSpecificDataFromDataBase(bsoncxx::document::view & doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityContainer::readSpecificDataFromDataBase(doc_view, entityMap);

	std::string categorySerialized = doc_view["Category"].get_utf8().value.to_string();
	DataCategorie category = UNKNOWN;
	_stringDataCategorieMapping.find(categorySerialized) == _stringDataCategorieMapping.end() ? category = UNKNOWN: category = _stringDataCategorieMapping.at(categorySerialized);
	if (category != UNKNOWN)
	{
		_selectedCategory = category;
	}
	else
	{
		throw std::runtime_error("Data category in EntityParameterizedDataCategorization was not set.");
	}
}

const std::string EntityParameterizedDataCategorization::_dataCategorieGroup = "Data Category";
const std::string EntityParameterizedDataCategorization::_defaultCategory = "DefaultCategory";
const std::string EntityParameterizedDataCategorization::_dataCategorieRMD = "Research";
const std::string EntityParameterizedDataCategorization::_dataCategorieMSMD = "Measurementseries";
const std::string EntityParameterizedDataCategorization::_dataCategorieParam = "Parameter";
const std::string EntityParameterizedDataCategorization::_dataCategorieQuant = "Quantity";

std::map<EntityParameterizedDataCategorization::DataCategorie, std::string> EntityParameterizedDataCategorization::_dataCategorieStringMapping = { {DataCategorie::researchMetadata, _dataCategorieRMD}, { DataCategorie::measurementSeriesMetadata,_dataCategorieMSMD }, { DataCategorie::parameter, _dataCategorieParam }, { DataCategorie::quantity, _dataCategorieQuant } };
std::map<std::string, EntityParameterizedDataCategorization::DataCategorie> EntityParameterizedDataCategorization::_stringDataCategorieMapping = { {_dataCategorieRMD, DataCategorie::researchMetadata }, { _dataCategorieMSMD, DataCategorie::measurementSeriesMetadata }, { _dataCategorieParam, DataCategorie::parameter }, { _dataCategorieQuant, DataCategorie::quantity } };

std::string EntityParameterizedDataCategorization::GetStringDataCategorization(EntityParameterizedDataCategorization::DataCategorie category)
{
	std::string returnVal;
	_dataCategorieStringMapping.find(category) == _dataCategorieStringMapping.end() ? returnVal = "" : returnVal = _dataCategorieStringMapping.at(category);
	return returnVal;
}
