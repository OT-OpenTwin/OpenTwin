//! @file TableCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/TableCfg.h"
#include "OTCore/VariableToStringConverter.h"

namespace ot::TableHeaderModeNames{
	const std::string ModeNameHorizontal = "First row";
	const std::string ModeNameVertical = "First column";
	const std::string ModeNameNone = "None";
}

std::string ot::TableCfg::toString(ot::TableCfg::TableHeaderMode _headerMode) {
	switch (_headerMode) {
	case ot::TableCfg::TableHeaderMode::NoHeader: return TableHeaderModeNames::ModeNameNone;
	case ot::TableCfg::TableHeaderMode::Horizontal: return TableHeaderModeNames::ModeNameHorizontal;
	case ot::TableCfg::TableHeaderMode::Vertical: return TableHeaderModeNames::ModeNameVertical;
	default:
		OT_LOG_EAS("Unknown header mode (" + std::to_string((int)_headerMode) + ")");
		return TableHeaderModeNames::ModeNameNone;
	}
}

ot::TableCfg::TableHeaderMode ot::TableCfg::stringToHeaderMode(const std::string& _headerMode) {
	if (_headerMode == ot::TableHeaderModeNames::ModeNameNone) return TableHeaderMode::NoHeader;
	else if (_headerMode == ot::TableHeaderModeNames::ModeNameHorizontal) return TableHeaderMode::Horizontal;
	else if (_headerMode == TableHeaderModeNames::ModeNameVertical) return TableHeaderMode::Vertical;
	else {
		OT_LOG_EAS("Unknown header mode \"" + _headerMode + "\"");
		return TableHeaderMode::NoHeader;
	}
}

ot::TableCfg::TableCfg(int _rows, int _columns, WidgetViewBase _baseInfo)
	: WidgetViewBase(_baseInfo), m_rows(_rows), m_columns(_columns)
{
	this->initialize();
}

ot::TableCfg::TableCfg(const ot::GenericDataStructMatrix& _matrix, TableCfg::TableHeaderMode _headerMode)
	: WidgetViewBase(WidgetViewBase::ViewTable, WidgetViewBase::ViewIsCentral | WidgetViewBase::ViewIsCloseable), m_rows(_matrix.getNumberOfRows()), m_columns(_matrix.getNumberOfColumns())
{
	MatrixEntryPointer matrixPointer;
	ot::VariableToStringConverter converter;

	int rowStarter = (_headerMode == TableHeaderMode::Horizontal ? 1 : 0);
	int columnStarter = (_headerMode == TableHeaderMode::Vertical ? 1 : 0);
	
	this->initialize(_matrix.getNumberOfRows() - rowStarter, _matrix.getNumberOfColumns() - columnStarter);

	if (rowStarter < m_rows && rowStarter > 0) {
		matrixPointer.m_row = 0;
		for (matrixPointer.m_column = 0; (int)matrixPointer.m_column < m_columns; matrixPointer.m_column++) {
			const Variable& variable = _matrix.getValue(matrixPointer);
			const std::string entry = converter(variable);
			this->setColumnHeader(matrixPointer.m_column, entry);
		}
	}
	if (columnStarter < m_columns && columnStarter > 0) {
		matrixPointer.m_column = 0;
		for (matrixPointer.m_row = 0; (int)matrixPointer.m_row < m_rows; matrixPointer.m_row++) {
			const Variable& variable = _matrix.getValue(matrixPointer);
			const std::string entry = converter(variable);
			this->setRowHeader(matrixPointer.m_row, entry);
		}
	}

	for(matrixPointer.m_column = columnStarter; (int)matrixPointer.m_column < _matrix.getNumberOfColumns(); matrixPointer.m_column++) {
		for (matrixPointer.m_row = rowStarter; (int)matrixPointer.m_row < _matrix.getNumberOfRows(); matrixPointer.m_row++) {
			const Variable& variable = _matrix.getValue(matrixPointer);
			const std::string entry = converter(variable);	
			this->setCellText(matrixPointer.m_row - rowStarter, matrixPointer.m_column - columnStarter, entry);
		}
	}	
}

ot::TableCfg::TableCfg(const TableCfg& _other) 
	: WidgetViewBase(_other), m_rows(0), m_columns(0)
{
	*this = _other;
}

ot::TableCfg::~TableCfg() {
	this->clear();
}

ot::TableCfg& ot::TableCfg::operator = (const TableCfg& _other) {
	if (this == &_other) return *this;
	WidgetViewBase::operator=(_other);

	// Clear data
	this->clear();

	// Initialize data
	m_rows = _other.m_rows;
	m_columns = _other.m_columns;

	this->initialize();

	// Copy table
	for (int r = 0; r < m_rows; r++) {
		for (int c = 0; c < m_columns; c++) {
			m_data[r][c] = _other.m_data[r][c];
		}
	}

	// Copy header
	for (int i = 0; i < m_rows; i++) {
		OTAssert(i < m_rowHeader.size(), "Index out of range");
		if (_other.m_rowHeader[i]) {
			m_rowHeader[i] = new TableHeaderItemCfg(*_other.m_rowHeader[i]);
		}
	}
	for (int i = 0; i < m_columns; i++) {
		OTAssert(i < m_columnHeader.size(), "Index out of range");
		if (_other.m_columnHeader[i]) {
			m_columnHeader[i] = new TableHeaderItemCfg(*_other.m_columnHeader[i]);
		}
	}

	return *this;
}

void ot::TableCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	WidgetViewBase::addToJsonObject(_object, _allocator);

	_object.AddMember("Rows", m_rows, _allocator);
	_object.AddMember("Columns", m_columns, _allocator);

	// Row header
	JsonArray rowHeaderArr;
	for (TableHeaderItemCfg* itm : m_rowHeader) {
		if (itm) {
			JsonObject itmObj;
			itm->addToJsonObject(itmObj, _allocator);
			rowHeaderArr.PushBack(itmObj, _allocator);
		}
		else {
			rowHeaderArr.PushBack(JsonNullValue(), _allocator);
		}
	}
	_object.AddMember("RowHeader", rowHeaderArr, _allocator);

	// Column header
	JsonArray columnHeaderArr;
	for (TableHeaderItemCfg* itm : m_columnHeader) {
		if (itm) {
			JsonObject itmObj;
			itm->addToJsonObject(itmObj, _allocator);
			columnHeaderArr.PushBack(itmObj, _allocator);
		}
		else {
			columnHeaderArr.PushBack(JsonNullValue(), _allocator);
		}
	}
	_object.AddMember("ColumnHeader", columnHeaderArr, _allocator);

	// Data
	JsonArray dataArr;
	for (const std::vector<std::string>& column : m_data) {
		dataArr.PushBack(JsonArray(column, _allocator), _allocator);
	}
	_object.AddMember("Data", dataArr, _allocator);
}

void ot::TableCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	WidgetViewBase::setFromJsonObject(_object);

	this->clear();

	m_rows = json::getInt(_object, "Rows");
	m_columns = json::getInt(_object, "Columns");

	this->initialize();

	// Row header
	ConstJsonArray rowHeaderArr = json::getArray(_object, "RowHeader");
	for (JsonSizeType i = 0; i < rowHeaderArr.Size(); i++) {
		OTAssert((int)i < m_rows, "Index out of range");
		if (rowHeaderArr[i].IsObject()) {
			ConstJsonObject itmObj = json::getObject(rowHeaderArr, i);
			TableHeaderItemCfg* newItem = new TableHeaderItemCfg;
			newItem->setFromJsonObject(itmObj);
			m_rowHeader[i] = newItem;
		}
	}

	// Column header
	ConstJsonArray columnHeaderArr = json::getArray(_object, "ColumnHeader");
	for (JsonSizeType i = 0; i < columnHeaderArr.Size(); i++) {
		OTAssert((int)i < m_columns, "Index out of range");
		if (columnHeaderArr[i].IsObject()) {
			ConstJsonObject itmObj = json::getObject(columnHeaderArr, i);
			TableHeaderItemCfg* newItem = new TableHeaderItemCfg;
			newItem->setFromJsonObject(itmObj);
			m_columnHeader[i] = newItem;
		}
	}

	// Data
	ConstJsonArray dataArr = json::getArray(_object, "Data");
	for (JsonSizeType r = 0; r < dataArr.Size(); r++) {
		OTAssert((int)r < m_rows, "Index out of range");

		ConstJsonArray columnArr = json::getArray(dataArr, r);
		for (JsonSizeType c = 0; c < columnArr.Size(); c++) {
			OTAssert((int)c < m_columns, "Index out of range");
			m_data[r][c] = json::getString(columnArr, c);
		}
	}

}

void ot::TableCfg::clear(void) {
	// Clear data
	for (TableHeaderItemCfg* itm : m_rowHeader) {
		if (itm) delete itm;
	}
	m_rowHeader.clear();

	for (TableHeaderItemCfg* itm : m_columnHeader) {
		if (itm) delete itm;
	}
	m_columnHeader.clear();

	m_data.clear();

	m_rows = 0;
	m_columns = 0;
}

void ot::TableCfg::setCellText(int _row, int _column, const std::string& _text) {
	OTAssert(_row < m_rows, "Index out of range");
	OTAssert(_column < m_columns, "Index out of range");
	m_data[_row][_column] = _text;
}

const std::string& ot::TableCfg::getCellText(int _row, int _column) const {
	OTAssert(_row < m_rows, "Index out of range");
	OTAssert(_column < m_columns, "Index out of range");
	return m_data[_row][_column];
}

void ot::TableCfg::setRowHeader(int _row, const std::string& _headerText) {
	this->setRowHeader(_row, new TableHeaderItemCfg(_headerText));
}

void ot::TableCfg::setRowHeader(int _row, TableHeaderItemCfg* _item) {
	OTAssert(_row < m_rows, "Index out of range");
	if (m_rowHeader[_row] == _item) return;
	if (m_rowHeader[_row]) delete m_rowHeader[_row];
	m_rowHeader[_row] = _item;
}

const ot::TableHeaderItemCfg* ot::TableCfg::getRowHeader(int _row) const {
	OTAssert(_row < m_rows, "Index out of range");
	return m_rowHeader[_row];
}

void ot::TableCfg::setColumnHeader(int _column, const std::string& _headerText) {
	this->setColumnHeader(_column, new TableHeaderItemCfg(_headerText));
}

void ot::TableCfg::setColumnHeader(int _column, TableHeaderItemCfg* _item) {
	OTAssert(_column < m_columns, "Index out of range");
	if (m_columnHeader[_column] == _item) return;
	if (m_columnHeader[_column]) delete m_columnHeader[_column];
	m_columnHeader[_column] = _item;
}

const ot::TableHeaderItemCfg* ot::TableCfg::getColumnHeader(int _column) const {
	OTAssert(_column < m_columns, "Index out of range");
	return m_columnHeader[_column];
}

void ot::TableCfg::initialize(void) {
	OTAssert(m_rows >= 0, "Index out of range");
	OTAssert(m_columns >= 0, "Index out of range");

	for (int r = 0; r < m_rows; r++) {
		m_data.push_back(std::vector<std::string>(m_columns, std::string()));
	}

	m_rowHeader = std::move(std::vector<TableHeaderItemCfg*>(m_rows, nullptr));
	m_columnHeader = std::move(std::vector<TableHeaderItemCfg*>(m_columns, nullptr));
}

void ot::TableCfg::initialize(int _rows, int _columns)
{
	m_rows = _rows;
	m_columns = _columns;
	this->initialize();
}
