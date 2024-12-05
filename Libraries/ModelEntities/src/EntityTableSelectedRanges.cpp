#include "EntityTableSelectedRanges.h"
#include <bsoncxx/builder/basic/array.hpp>

#include "OTCore/TypeNames.h"
#include "OTCommunication/ActionTypes.h"

EntityTableSelectedRanges::EntityTableSelectedRanges(ot::UID ID, EntityBase * parent, EntityObserver * obs, ModelState * ms, ClassFactoryHandler* factory, const std::string & owner)
	:EntityBase(ID,parent,obs,ms,factory,owner)
{
}

void EntityTableSelectedRanges::addVisualizationNodes()
{
	if (!getName().empty())
	{
		OldTreeIcon treeIcons;
		treeIcons.size = 32;
		treeIcons.visibleIcon = "SelectedRange";
		treeIcons.hiddenIcon = "SelectedRange";

		ot::JsonDocument doc;
		doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_VIEW_AddContainerNode, doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_UI_TREE_Name, ot::JsonString(this->getName(), doc.GetAllocator()), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_EntityID, this->getEntityID(), doc.GetAllocator());
		doc.AddMember(OT_ACTION_PARAM_MODEL_ITM_IsEditable, this->getEditable(), doc.GetAllocator());

		treeIcons.addToJsonDoc(doc);

		getObserver()->sendMessageToViewer(doc);
	}

	EntityBase::addVisualizationNodes();
}

bool EntityTableSelectedRanges::updateFromProperties(void)
{

	// Now we need to update the entity after a property change
	assert(getProperties().anyPropertyNeedsUpdate());

	// Since there is a change now, we need to set the modified flag
	setModified();

	// Here we need to update the plot (send a message to the visualization service)
	getProperties().forceResetUpdateForAllProperties();
	
	return UpdateVisibility();
}

void EntityTableSelectedRanges::createProperties(const std::string& pythonScriptFolder, ot::UID pythonScriptFolderID, const std::string& pythonScriptName, ot::UID pythonScriptID, const std::string& _defaultType)
{
	std::string sourceFileGroup = "Source file";

	auto tableName = new EntityPropertiesString("Table name", "");
	tableName->setReadOnly(true);
	auto tableOrientation = new EntityPropertiesString("Header position", "");
	tableOrientation->setReadOnly(true);
	getProperties().createProperty(tableName,sourceFileGroup);
	getProperties().createProperty(tableOrientation,sourceFileGroup);
	
	std::string categoryGroup = "Data properties";
	EntityPropertiesSelection::createProperty(categoryGroup, "Datatype", { ot::TypeNames::getDoubleTypeName(),ot::TypeNames::getFloatTypeName(), ot::TypeNames::getInt64TypeName(), ot::TypeNames::getInt32TypeName(), ot::TypeNames::getStringTypeName() }, _defaultType, OT_INFO_SERVICE_TYPE_ImportParameterizedDataService, getProperties());

	const std::string rangeGroup = "Range";
	auto topRow = new EntityPropertiesInteger("Top row", 0);
	topRow->setReadOnly(true);
	auto bottomRow = new EntityPropertiesInteger("Bottom row", 0);
	bottomRow->setReadOnly(true);
	auto leftColumn = new EntityPropertiesInteger("Left column", 0);
	leftColumn->setReadOnly(true);
	auto rightColumn = new EntityPropertiesInteger("Right column", 0);
	rightColumn->setReadOnly(true);
	
	const std::string updateStrategyGroup = "Update strategy";
	auto considerInAutomaticCreation = new EntityPropertiesBoolean(_propNameConsiderForBatchProcessing, false);
	auto updateScript = new EntityPropertiesEntityList(_pythonScriptProperty, pythonScriptFolder, pythonScriptFolderID, pythonScriptName, pythonScriptID);
	updateScript->setVisible(false);
	
	auto passOnScript = new EntityPropertiesBoolean(_propNamePassOnScript, false);
	passOnScript->setVisible(false);

	getProperties().createProperty(topRow, rangeGroup);
	getProperties().createProperty(bottomRow, rangeGroup);
	getProperties().createProperty(leftColumn, rangeGroup);
	getProperties().createProperty(rightColumn, rangeGroup);
	

	getProperties().createProperty(considerInAutomaticCreation, updateStrategyGroup);
	getProperties().createProperty(updateScript, updateStrategyGroup);
	getProperties().createProperty(passOnScript, updateStrategyGroup);
	
	EntityPropertiesBoolean::createProperty(updateStrategyGroup, "Select entire row", false, "default", getProperties());
	EntityPropertiesBoolean::createProperty(updateStrategyGroup, "Select entire column", false, "default", getProperties());
}

void EntityTableSelectedRanges::setTableProperties(std::string _tableName, ot::UID _tableID, std::string _tableOrientation) {
	auto tableNameEnt = dynamic_cast<EntityPropertiesString*>(getProperties().getProperty("Table name"));
	auto headerPosEnt = dynamic_cast<EntityPropertiesString*>(getProperties().getProperty("Header position"));
	tableNameEnt->setValue(_tableName);
	headerPosEnt->setValue(_tableOrientation);
	_tableID = _tableID;
	setModified();
}


std::string EntityTableSelectedRanges::getSelectedType() {
	auto typeSelection = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty("Datatype"));
	return typeSelection->getValue();

}

std::string EntityTableSelectedRanges::getTableName() {
	auto tableNameEnt = dynamic_cast<EntityPropertiesString*>(getProperties().getProperty("Table name"));
	return tableNameEnt->getValue();
}

ot::TableCfg::TableHeaderMode EntityTableSelectedRanges::getTableHeaderMode() {
	auto modeProp = dynamic_cast<EntityPropertiesString*>(getProperties().getProperty("Header position"));
	ot::TableCfg::TableHeaderMode mode = ot::TableCfg::stringToHeaderMode(modeProp->getValue());
	return mode;
}

ot::TableRange EntityTableSelectedRanges::getSelectedRange()
{
	auto topRowEnt = dynamic_cast<EntityPropertiesInteger*>(getProperties().getProperty("Top row"));
	auto bottomRowEnt = dynamic_cast<EntityPropertiesInteger*>(getProperties().getProperty("Bottom row"));
	auto leftColumnEnt = dynamic_cast<EntityPropertiesInteger*>(getProperties().getProperty("Left column"));
	auto rightColumnEnt = dynamic_cast<EntityPropertiesInteger*>(getProperties().getProperty("Right column"));

	ot::TableRange tableRange(topRowEnt->getValue(), leftColumnEnt->getValue(), bottomRowEnt->getValue(), rightColumnEnt->getValue());
	return tableRange;
}

bool EntityTableSelectedRanges::getConsiderForBatchprocessing()
{
	auto considerForBatchProcessing = dynamic_cast<EntityPropertiesBoolean*>(getProperties().getProperty(_propNameConsiderForBatchProcessing));
	return considerForBatchProcessing->getValue();
}

void EntityTableSelectedRanges::setConsiderForBatchprocessing(bool considerForBatchprocessing)
{
	auto considerForBatchProcessing = dynamic_cast<EntityPropertiesBoolean*>(getProperties().getProperty(_propNameConsiderForBatchProcessing));
	considerForBatchProcessing->setValue(considerForBatchprocessing);
	UpdateVisibility();
}

bool EntityTableSelectedRanges::getSelectEntireColumn()
{
	EntityPropertiesBase* baseProperty = getProperties().getProperty("Select entire column");
	if (baseProperty != nullptr)
	{
		auto boolProperty = dynamic_cast<EntityPropertiesBoolean*>(baseProperty);
		return boolProperty->getValue();
	}
	else
	{
		return false;
	}
}

bool EntityTableSelectedRanges::getSelectEntireRow()
{
	EntityPropertiesBase* baseProperty = getProperties().getProperty("Select entire row");
	if (baseProperty != nullptr)
	{
		auto boolProperty = dynamic_cast<EntityPropertiesBoolean*>(baseProperty);
		return boolProperty->getValue();
	}
	else
	{
		return false;
	}
}

void EntityTableSelectedRanges::setSelectEntireColumn(bool _selectAll)
{
	EntityPropertiesBase* baseProperty = getProperties().getProperty("Select entire column");
	if (baseProperty != nullptr)
	{
		auto boolProperty = dynamic_cast<EntityPropertiesBoolean*>(baseProperty);
		boolProperty->setValue(_selectAll);
		setModified();
	}
}

void EntityTableSelectedRanges::setSelectEntireRow(bool _selectAll)
{
	EntityPropertiesBase* baseProperty = getProperties().getProperty("Select entire row");
	if (baseProperty != nullptr)
	{
		auto boolProperty = dynamic_cast<EntityPropertiesBoolean*>(baseProperty);
		boolProperty->setValue(_selectAll);
		setModified();
	}
}

bool EntityTableSelectedRanges::getPassOnScript()
{
	auto considerForBatchProcessing = dynamic_cast<EntityPropertiesBoolean*>(getProperties().getProperty(_propNamePassOnScript));
	return considerForBatchProcessing->getValue();
}

std::string EntityTableSelectedRanges::getScriptName()
{
	auto selectedScript = dynamic_cast<EntityPropertiesEntityList*>(getProperties().getProperty(_pythonScriptProperty));
	return selectedScript->getValueName();
}

void EntityTableSelectedRanges::setRange(const ot::TableRange& _range)
{
	if (_range.getBottomRow() >= _range.getTopRow() && _range.getRightColumn() >= _range.getLeftColumn())
	{
		auto topRowEnt = dynamic_cast<EntityPropertiesInteger*>(getProperties().getProperty("Top row"));
		auto bottomRowEnt = dynamic_cast<EntityPropertiesInteger*>(getProperties().getProperty("Bottom row"));
		auto leftColumnEnt = dynamic_cast<EntityPropertiesInteger*>(getProperties().getProperty("Left column"));
		auto rightColumnEnt = dynamic_cast<EntityPropertiesInteger*>(getProperties().getProperty("Right column"));
		topRowEnt->setValue(_range.getTopRow());
		bottomRowEnt->setValue(_range.getBottomRow());
		leftColumnEnt->setValue(_range.getLeftColumn());
		rightColumnEnt->setValue(_range.getRightColumn());
		setModified();
	}
	else
	{
		OT_LOG_E("Tried to set a table selection range with non fitting values.");
	}
}


bool EntityTableSelectedRanges::UpdateVisibility()
{
	auto showScriptSelection = dynamic_cast<EntityPropertiesBoolean*>(getProperties().getProperty(_propNameConsiderForBatchProcessing));
	auto scriptSelectionProperty = getProperties().getProperty(_pythonScriptProperty);
	auto passOnScriptProperty = getProperties().getProperty(_propNamePassOnScript);

	if (showScriptSelection->getValue() != scriptSelectionProperty->getVisible())
	{
		scriptSelectionProperty->setVisible(showScriptSelection->getValue());
		passOnScriptProperty->setVisible(showScriptSelection->getValue());
		return true;
	}
	else
	{
		return false;
	}
}

void EntityTableSelectedRanges::AddStorageData(bsoncxx::builder::basic::document& storage)
{
	storage.append(
		bsoncxx::builder::basic::kvp("TableID", static_cast<int64_t>(_tableID))
	);
	
}

void EntityTableSelectedRanges::readSpecificDataFromDataBase(bsoncxx::document::view& doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);
	_tableID = static_cast<ot::UID>(doc_view["TableID"].get_int64());
}

bool EntityTableSelectedRanges::getEntityBox(double & xmin, double & xmax, double & ymin, double & ymax, double & zmin, double & zmax)
{
	return false;
}
