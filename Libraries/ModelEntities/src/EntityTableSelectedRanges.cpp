#include "EntityTableSelectedRanges.h"
#include <bsoncxx/builder/basic/array.hpp>

#include <OpenTwinCore/TypeNames.h>
#include <OpenTwinCommunication/ActionTypes.h>

EntityTableSelectedRanges::EntityTableSelectedRanges(ot::UID ID, EntityBase * parent, EntityObserver * obs, ModelState * ms, ClassFactoryHandler* factory, const std::string & owner)
	:EntityBase(ID,parent,obs,ms,factory,owner)
{
}

void EntityTableSelectedRanges::addVisualizationNodes()
{
	if (!getName().empty())
	{
		TreeIcon treeIcons;
		treeIcons.size = 32;
		treeIcons.visibleIcon = "SelectedRange";
		treeIcons.hiddenIcon = "SelectedRange";

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

bool EntityTableSelectedRanges::updateFromProperties(void)
{

	// Now we need to update the entity after a property change
	assert(getProperties().anyPropertyNeedsUpdate());

	// Since there is a change now, we need to set the modified flag
	setModified();

	// Here we need to update the plot (send a message to the visualization service)
	getProperties().forceResetUpdateForAllProperties();
	
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

void EntityTableSelectedRanges::createProperties(const std::string& pythonScriptFolder, ot::UID pythonScriptFolderID, const std::string& pythonScriptName, ot::UID pythonScriptID, bool selectEntireRow , bool selectEntireColumn)
{
	std::string sourceFileGroup = "Source file";

	auto tableName = new EntityPropertiesString("Table name", _tableName);
	tableName->setReadOnly(true);
	auto tableOrientation = new EntityPropertiesString("Header position", _tableOrientation);
	tableOrientation->setReadOnly(true);
	getProperties().createProperty(tableName,sourceFileGroup);
	getProperties().createProperty(tableOrientation,sourceFileGroup);
	
	std::string categoryGroup = "Data properties";
	EntityPropertiesSelection::createProperty(categoryGroup, "Datatype", { ot::TypeNames::getDoubleTypeName(), ot::TypeNames::getInt64TypeName(), ot::TypeNames::getInt32TypeName(), ot::TypeNames::getStringTypeName() }, ot::TypeNames::getStringTypeName(), OT_INFO_SERVICE_TYPE_ImportParameterizedDataService, getProperties());

	const std::string rangeGroup = "Range";
	auto topRow = new EntityPropertiesInteger("Top row", _topCells);
	topRow->setReadOnly(true);
	auto bottomRow = new EntityPropertiesInteger("Bottom row", _buttomCells);
	bottomRow->setReadOnly(true);
	auto leftColumn = new EntityPropertiesInteger("Left column", _leftCells);
	leftColumn->setReadOnly(true);
	auto rightColumn = new EntityPropertiesInteger("Right column", _rightCells);
	rightColumn->setReadOnly(true);
	
	const std::string updateStrategyGroup = "Update strategy";
	auto considerInAutomaticCreation = new EntityPropertiesBoolean(_propNameConsiderForBatchProcessing, false);
	auto updateScript = new EntityPropertiesEntityList(_pythonScriptProperty, pythonScriptFolder, pythonScriptFolderID, pythonScriptName, pythonScriptID);
	updateScript->setVisible(false);
	
	auto passOnScript = new EntityPropertiesBoolean(_propNamePassOnScript, false);
	passOnScript->setVisible(false);


	EntityPropertiesBoolean::createProperty(rangeGroup, "Select entire row", selectEntireRow, "default", getProperties());
	EntityPropertiesBoolean::createProperty(rangeGroup, "Select entire column", selectEntireColumn, "default", getProperties());

	getProperties().createProperty(topRow, rangeGroup);
	getProperties().createProperty(bottomRow, rangeGroup);
	getProperties().createProperty(leftColumn, rangeGroup);
	getProperties().createProperty(rightColumn, rangeGroup);
	

	getProperties().createProperty(considerInAutomaticCreation, updateStrategyGroup);
	getProperties().createProperty(updateScript, updateStrategyGroup);
	getProperties().createProperty(passOnScript, updateStrategyGroup);
}

void EntityTableSelectedRanges::SetTableProperties(std::string tableName, ot::UID tableID, ot::UID tableVersion, std::string tableOrientation)
{
	_tableName = tableName;
	_tableID = tableID;
	_tableVersion = tableVersion;
	_tableOrientation = tableOrientation;
}


std::string EntityTableSelectedRanges::getSelectedType()
{
	auto typeSelection = dynamic_cast<EntityPropertiesSelection*>(getProperties().getProperty("Datatype"));
	return typeSelection->getValue();

}

void EntityTableSelectedRanges::getSelectedRange(uint32_t& topRow, uint32_t& bottomRow, uint32_t& leftColumn, uint32_t& rightColumn, std::shared_ptr<EntityParameterizedDataTable> referencedTable)
{
	getSelectedRange(topRow, bottomRow, leftColumn, rightColumn);
	if (getSelectEntireColumn())
	{
		bottomRow = referencedTable->getTableData()->getNumberOfRows() -1;
		if (referencedTable->getSelectedHeaderOrientation() == EntityParameterizedDataTable::HeaderOrientation::horizontal)
		{
			topRow = 1;
		}
		else
		{
			topRow = 0;
		}
	}
	if (getSelectEntireRow())
	{
		rightColumn = referencedTable->getTableData()->getNumberOfColumns() - 1;
		if (referencedTable->getSelectedHeaderOrientation() == EntityParameterizedDataTable::HeaderOrientation::vertical)
		{
			leftColumn = 1;
		}
		else
		{
			leftColumn = 0;
		}
	}

}

void EntityTableSelectedRanges::getSelectedRange(uint32_t& topRow, uint32_t& bottomRow, uint32_t& leftColumn, uint32_t& rightColumn)
{
	topRow = _topCells;
	bottomRow = _buttomCells;
	leftColumn = _leftCells;
	rightColumn = _rightCells;
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
}

bool EntityTableSelectedRanges::getSelectEntireRow()
{
	auto selectEntireRowEnt = dynamic_cast<EntityPropertiesBoolean*>(getProperties().getProperty("Select entire row"));
	return selectEntireRowEnt->getValue();
}

bool EntityTableSelectedRanges::getSelectEntireColumn()
{
	auto selectEntireRowEnt = dynamic_cast<EntityPropertiesBoolean*>(getProperties().getProperty("Select entire column"));
	return selectEntireRowEnt->getValue();
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

void EntityTableSelectedRanges::AddRange(uint32_t topCell, uint32_t buttomCell, uint32_t leftCell, uint32_t rightCell)
{
	if (topCell >= topCell && rightCell >= leftCell)
	{
		_topCells = topCell;
		_buttomCells = buttomCell;
		_leftCells = leftCell;
		_rightCells = rightCell;
	}
}

void EntityTableSelectedRanges::AddStorageData(bsoncxx::builder::basic::document & storage)
{
	EntityBase::AddStorageData(storage);

	storage.append(
		bsoncxx::builder::basic::kvp("TopCells", static_cast<int32_t>(_topCells)),
		bsoncxx::builder::basic::kvp("ButtomCells", static_cast<int32_t>(_buttomCells)),
		bsoncxx::builder::basic::kvp("LeftCells", static_cast<int32_t>(_leftCells)),
		bsoncxx::builder::basic::kvp("RightCells", static_cast<int32_t>(_rightCells)),
		bsoncxx::builder::basic::kvp("TableName", _tableName),
		bsoncxx::builder::basic::kvp("TableOrientierung", _tableOrientation)
	);
}

void EntityTableSelectedRanges::readSpecificDataFromDataBase(bsoncxx::document::view & doc_view, std::map<ot::UID, EntityBase*>& entityMap)
{
	EntityBase::readSpecificDataFromDataBase(doc_view, entityMap);

	_topCells = static_cast<uint32_t>(doc_view["TopCells"].get_int32().value);
	_buttomCells = static_cast<uint32_t>(doc_view["ButtomCells"].get_int32().value);
	_leftCells = static_cast<uint32_t>(doc_view["LeftCells"].get_int32().value);
	_rightCells = static_cast<uint32_t>(doc_view["RightCells"].get_int32().value);

	_tableName = doc_view["TableName"].get_utf8().value.data();
	_tableOrientation = doc_view["TableOrientierung"].get_utf8().value.data();
}

bool EntityTableSelectedRanges::getEntityBox(double & xmin, double & xmax, double & ymin, double & ymax, double & zmin, double & zmax)
{
	return false;
}
