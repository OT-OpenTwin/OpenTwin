#include "EntityBuffer.h"
#include "Application.h"
#include "IVisualisationTable.h"

#include "PropertyPythonObjectConverter.h"
#include "PythonObjectBuilder.h"
#include "EntityTableSelectedRanges.h"
#include "OTServiceFoundation/TableIndexSchemata.h"

EntityBuffer& EntityBuffer::instance() {
	static EntityBuffer g_instance;
	return g_instance;
}

void EntityBuffer::setModelServiceAPI(ot::ModelServiceAPI* _modelServiceAPI) {
	assert(_modelServiceAPI != nullptr);
	m_modelServiceAPI = _modelServiceAPI;
}

PyObject* EntityBuffer::getEntityPropertyValue(const std::string& _absoluteEntityName, const std::string& _propertyName)
{
	ensurePropertyToBeLoaded(_absoluteEntityName, _propertyName);
	PropertyPythonObjectConverter interface(m_bufferedEntityProperties[_absoluteEntityName + _propertyName]);
	return interface.GetValue();
}

PyObject* EntityBuffer::getTableCellValue(const std::string& _absoluteEntityName, uint32_t _row, uint32_t _column)
{
	ensureTableToBeLoaded(_absoluteEntityName);
	auto tableData = m_bufferedTableEntities[_absoluteEntityName]->getTable(); 
	
	ot::MatrixEntryPointer matrixEntry;
	matrixEntry.m_column = _column;
	matrixEntry.m_row = _row;
	if(_row < tableData.getNumberOfRows() && _column < tableData.getNumberOfColumns())
	{
		tableData.getNumberOfColumns();
		ot::Variable cellValue  = tableData.getValue(matrixEntry);
		assert(cellValue.isConstCharPtr());

		PythonObjectBuilder builder;
		auto pCellValue = builder.setString(cellValue.getConstCharPtr());
		Py_INCREF(pCellValue);
		return pCellValue;
	}
	else
	{
		throw std::exception("Cell access out of range");
	}
}

void EntityBuffer::updateEntityPropertyValue(const std::string& _absoluteEntityName, const std::string& _propertyName, const CPythonObject& _values)
{
	ensurePropertyToBeLoaded(_absoluteEntityName, _propertyName);
	PropertyPythonObjectConverter interface(m_bufferedEntityProperties[_absoluteEntityName + _propertyName]);
	interface.SetValue(_values);
}

void EntityBuffer::saveChangedEntities()
{
	ot::UIDList topoEntID, topoEntVersion, dataEnt;
	std::list<bool> forceVis;
	for (auto bufferedEntity : m_bufferedEntities)
	{
		ensureValidRangeSelections(bufferedEntity.second.get());
		auto entity = bufferedEntity.second;
		entity->StoreToDataBase();
		topoEntID.push_back(entity->getEntityID());
		topoEntVersion.push_back(entity->getEntityStorageVersion());
		forceVis.push_back(false);
	}

	if (topoEntID.size() > 0)
	{
		m_modelServiceAPI->addEntitiesToModel(topoEntID, topoEntVersion, forceVis, dataEnt, dataEnt, dataEnt, "Entity property update by python execution service");
		m_modelServiceAPI->updatePropertyGrid();
	}
	clearBuffer();
}

bool EntityBuffer::saveChangedEntities(const std::string& _absoluteEntityName)
{
	if (m_bufferedEntities.find(_absoluteEntityName) != m_bufferedEntities.end())
	{
		std::shared_ptr<EntityBase> entity = m_bufferedEntities[_absoluteEntityName];
		entity->StoreToDataBase();
		ot::UIDList topoEntID, topoEntVersion, dataEnt;
		std::list<bool> forceVis;
		topoEntID.push_back(entity->getEntityID());
		topoEntVersion.push_back(entity->getEntityStorageVersion());
		m_modelServiceAPI->addEntitiesToModel(topoEntID, topoEntVersion, forceVis, dataEnt, dataEnt, dataEnt, "Entity property update by python execution service");
		m_bufferedEntities.erase(_absoluteEntityName);
		return true;
	}
	return false;
}

std::shared_ptr<EntityBase> EntityBuffer::getEntity(const std::string& _absoluteEntityName)
{
	return loadEntity(_absoluteEntityName);
}


void EntityBuffer::ensurePropertyToBeLoaded(const std::string& _absoluteEntityName, const std::string& _propertyName)
{
	if (m_bufferedEntityProperties.find(_absoluteEntityName + _propertyName) == m_bufferedEntityProperties.end())
	{
		std::shared_ptr<EntityBase> entity = loadEntity(_absoluteEntityName);
		EntityPropertiesBase* property = entity->getProperties().getProperty(_propertyName);
		if (property == nullptr)
		{
			throw std::exception(("Requested property " + _propertyName + " does not exist in entity " + _absoluteEntityName).c_str());
		}
		m_bufferedEntityProperties[_absoluteEntityName + _propertyName] = property;
	}
}

void EntityBuffer::ensureTableToBeLoaded(const std::string& _absoluteEntityName)
{
	if (m_bufferedTableEntities.find(_absoluteEntityName) == m_bufferedTableEntities.end())
	{
		std::shared_ptr<EntityBase> entity = loadEntity(_absoluteEntityName);
		IVisualisationTable* newResultTable(dynamic_cast<IVisualisationTable*>(entity.get()));
		if (newResultTable == nullptr)
		{
			throw std::exception(("Requested table " + _absoluteEntityName + " is not a table.").c_str());
		}
		m_bufferedTableEntities[_absoluteEntityName] = newResultTable;
	}
}

void EntityBuffer::ensureValidRangeSelections(EntityBase* _entityBase) {
	EntityTableSelectedRanges* range = dynamic_cast<EntityTableSelectedRanges*>(_entityBase);
	if (range != nullptr) {
		const std::string tableName = range->getTableName();
		bool exists = true;
		try {
			ensureTableToBeLoaded(tableName);
		}
		catch (...) {
			exists = false;
		}
		if (!exists) {
			OT_LOG_E("Range selection \"" + range->getName() + "\" references a non existing table: "+ tableName);
		}
		else {

			auto tableByName = m_bufferedTableEntities.find(tableName);
			IVisualisationTable* table = tableByName->second;
			ot::GenericDataStructMatrix tableContent =	table->getTable();
			int32_t maxNumberOfRows = static_cast<int32_t>(tableContent.getNumberOfRows());
			int32_t maxNumberOfColumns = static_cast<int32_t>(tableContent.getNumberOfColumns());

			ot::TableRange selectedRange =	range->getSelectedRange();
			ot::TableRange matrixRange = ot::TableIndexSchemata::userRangeToMatrixRange(selectedRange,table->getHeaderMode());
		
			bool requiresChange = false;
			std::string changedMessage("");
		
			int32_t minColumn(0), minRow(0);
			if(range->getTableHeaderMode() == ot::TableCfg::TableHeaderMode::Horizontal)
			{
				minRow = 1;
			}
			else if (range->getTableHeaderMode() == ot::TableCfg::TableHeaderMode::Vertical)
			{
				minColumn = 1;
			}

			//Now we check if the selected ranges are in an unsuported range
			if (matrixRange.getBottomRow() >= maxNumberOfRows) //Matrix range is a zero-based index
			{
				matrixRange.setBottomRow(maxNumberOfRows - 1);
				requiresChange = true;
				changedMessage += "Bottom row: " + std::to_string(selectedRange.getBottomRow());
			}
			else if (matrixRange.getBottomRow() < minRow)
			{
				matrixRange.setBottomRow(minRow);
				requiresChange = true;
				changedMessage += "Bottom row: " + std::to_string(selectedRange.getBottomRow());
			}
		
			if (matrixRange.getTopRow() < minRow)
			{
				matrixRange.setTopRow(minRow);
				changedMessage += "Top row: " + std::to_string(selectedRange.getTopRow());
				requiresChange = true;
			}
			else if (matrixRange.getTopRow() >= maxNumberOfRows)
			{
				matrixRange.setTopRow(maxNumberOfRows -1);
				changedMessage += "Top row: " + std::to_string(selectedRange.getTopRow());
				requiresChange = true;
			}

			if (matrixRange.getRightColumn() >= maxNumberOfColumns)
			{
				matrixRange.setRightColumn(maxNumberOfColumns - 1);
				requiresChange = true;
				changedMessage += "Right column: " + std::to_string(selectedRange.getRightColumn());
			}
			else if (matrixRange.getRightColumn() < minColumn)
			{
				matrixRange.setRightColumn(minColumn);
				requiresChange = true;
				changedMessage += "Right column: " + std::to_string(selectedRange.getRightColumn());
			}

			if (matrixRange.getLeftColumn() < minColumn)
			{
				matrixRange.setLeftColumn(minColumn);
				requiresChange = true;
				changedMessage += "Left column: " + std::to_string(selectedRange.getLeftColumn());
			}
			else if (matrixRange.getLeftColumn() >= maxNumberOfColumns)
			{
				matrixRange.setLeftColumn(maxNumberOfColumns -1);
				requiresChange = true;
				changedMessage += "Left column: " + std::to_string(selectedRange.getLeftColumn());
			}

			if (requiresChange)
			{
				changedMessage = "Range selection \"" + range->getName() + " \" had its range adjusted outside the possible values. The adjusted values are capped accordingly. The value(s) out of range was(were): " +
					changedMessage.substr(changedMessage.size() - 2);
				OT_LOG_W(changedMessage);
				range->setRange(ot::TableIndexSchemata::matrixToUserRange(matrixRange, range->getTableHeaderMode()));
			}
		}
	}
}

std::shared_ptr<EntityBase> EntityBuffer::loadEntity(const std::string& _absoluteEntityName)
{
	if (m_bufferedEntities.find(_absoluteEntityName) == m_bufferedEntities.end())
	{
		ot::EntityInformation entityInfo;

		m_modelServiceAPI->getEntityInformation(_absoluteEntityName, entityInfo);
		if (entityInfo.getEntityName() == "")
		{
			throw std::exception(("Requested entity " + _absoluteEntityName + " does not exist.").c_str());
		}
		EntityBase* entity = m_modelServiceAPI->readEntityFromEntityIDandVersion(entityInfo.getEntityID(), entityInfo.getEntityVersion(), Application::instance().getClassFactory());
		m_bufferedEntities[_absoluteEntityName] = std::shared_ptr<EntityBase>(entity);
	}
	return m_bufferedEntities[_absoluteEntityName];
}

void EntityBuffer::clearBuffer()
{
	m_bufferedEntities.clear();
	m_bufferedEntityProperties.clear();
	m_bufferedTableEntities.clear();
}

EntityBuffer::EntityBuffer()
	: m_modelServiceAPI(nullptr)
{

}