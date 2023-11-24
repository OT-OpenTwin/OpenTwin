//! @file GraphicsGridLayoutItemCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/GraphicsGridLayoutItemCfg.h"

#define OT_JSON_MEMBER_Rows "Rows"
#define OT_JSON_MEMBER_Items "Items"
#define OT_JSON_MEMBER_Columns "Columns"
#define OT_JSON_MEMBER_RowStretch "RowStretch"
#define OT_JSON_MEMBER_ColumnStretch "ColumnStretch"

static ot::SimpleFactoryRegistrar<ot::GraphicsGridLayoutItemCfg> gridCfg(OT_SimpleFactoryJsonKeyValue_GraphicsGridLayoutItemCfg);

ot::GraphicsGridLayoutItemCfg::GraphicsGridLayoutItemCfg(int _rows, int _columns)
	: m_rows(_rows), m_columns(_columns)
{
	this->clearAndResize();
}

ot::GraphicsGridLayoutItemCfg::~GraphicsGridLayoutItemCfg() {
	for (auto r : m_items) {
		for (auto c : r) {
			if (c) delete c;
		}
	}
	m_items.clear();
}

void ot::GraphicsGridLayoutItemCfg::addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const {
	ot::GraphicsLayoutItemCfg::addToJsonObject(_document, _object);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Rows, m_rows);
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Columns, m_columns);

	OT_rJSON_createValueArray(rowStretchArr);
	for (auto r : m_rowStretch) {
		rowStretchArr.PushBack(r, _document.GetAllocator());
	}
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_RowStretch, rowStretchArr);

	OT_rJSON_createValueArray(columnStretchArr);
	for (auto c : m_columnStretch) {
		columnStretchArr.PushBack(c, _document.GetAllocator());
	}
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_ColumnStretch, columnStretchArr);

	OT_rJSON_createValueArray(itemArr);
	for (auto r : m_items) {
		OT_rJSON_createValueArray(columnArr);
		for (auto c : r) {
			if (c) {
				OT_rJSON_createValueObject(itemObj);
				c->addToJsonObject(_document, itemObj);
				columnArr.PushBack(itemObj, _document.GetAllocator());
			}
			else {
				OT_rJSON_createValueNull(itemObj);
				columnArr.PushBack(itemObj, _document.GetAllocator());
			}
		}
		itemArr.PushBack(columnArr, _document.GetAllocator());
	}
	ot::rJSON::add(_document, _object, OT_JSON_MEMBER_Items, itemArr);
}

void ot::GraphicsGridLayoutItemCfg::setFromJsonObject(OT_rJSON_val& _object) {
	ot::GraphicsLayoutItemCfg::setFromJsonObject(_object);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Rows, Int);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Columns, Int);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_RowStretch, Array);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_ColumnStretch, Array);
	OT_rJSON_checkMember(_object, OT_JSON_MEMBER_Items, Array);

	m_rows = _object[OT_JSON_MEMBER_Rows].GetInt();
	m_columns = _object[OT_JSON_MEMBER_Columns].GetInt();

	// Clear and resize the arrays
	this->clearAndResize();

	OT_rJSON_val rowStretchArr = _object[OT_JSON_MEMBER_RowStretch].GetArray();
	OT_rJSON_val columnStretchArr = _object[OT_JSON_MEMBER_ColumnStretch].GetArray();
	OT_rJSON_val itemsArr = _object[OT_JSON_MEMBER_Items].GetArray();

	// Ensure correct array sizes
	if (rowStretchArr.Size() != m_rows) {
		OT_LOG_EA("Row stretch array size mismatch");
		return;
	}
	if (columnStretchArr.Size() != m_columns) {
		OT_LOG_EA("Column stretch array size mismatch");
		return;
	}
	if (itemsArr.Size() != m_rows) {
		OT_LOG_EA("Item array size mismatch");
		return;
	}

	// Stretch factors
	for (rapidjson::SizeType i = 0; i < rowStretchArr.Size(); i++) {
		OT_rJSON_checkArrayEntryType(rowStretchArr, i, Int);
		m_rowStretch[i] = rowStretchArr[i].GetInt();
	}
	for (rapidjson::SizeType i = 0; i < columnStretchArr.Size(); i++) {
		OT_rJSON_checkArrayEntryType(columnStretchArr, i, Int);
		m_columnStretch[i] = columnStretchArr[i].GetInt();
	}

	// Items
	for (rapidjson::SizeType r = 0; r < itemsArr.Size(); r++) {
		OT_rJSON_checkArrayEntryType(itemsArr, r, Array);
		OT_rJSON_val columnArr = itemsArr[r].GetArray();
		if (columnArr.Size() != m_columns) {
			OT_LOG_EA("Item column array size mismatch");
			return;
		}
		for (rapidjson::SizeType c = 0; c < columnArr.Size(); c++) {
			if (columnArr[c].IsNull()) {
				m_items[r][c] = nullptr;
				continue;
			}
			else if (columnArr[c].IsObject()) {
				GraphicsItemCfg* itm = nullptr;
				try {
					OT_rJSON_val itemObj = columnArr[c].GetObject();
					itm = ot::SimpleFactory::instance().createType<GraphicsItemCfg>(itemObj);
					if (itm) {
						itm->setFromJsonObject(itemObj);
					}
					else {
						OT_LOG_EA("Factory failed");
					}
					m_items[r][c] = itm;
				}
				catch (const std::exception& _e) {
					if (itm) delete itm;
					throw _e;
				}
				catch (...) {
					if (itm) delete itm;
					OT_LOG_EA("Unknown error occured");
					throw std::exception("Unknown error");
				}
			}
		}
	}

}

void ot::GraphicsGridLayoutItemCfg::addChildItem(ot::GraphicsItemCfg* _item) {
	for (size_t r = 0; r < m_items.size(); r++) {
		for (size_t c = 0; c < m_items[r].size(); c++) {
			if (m_items[r][c] == nullptr) {
				m_items[r][c] = _item;
				return;
			}
		}
	}
	OT_LOG_EA("No more free space to add the item");
}

void ot::GraphicsGridLayoutItemCfg::addChildItem(int _row, int _column, ot::GraphicsItemCfg* _item) {
	if (_row < 0 || _row >= m_rows) {
		OT_LOG_EA("Row index out of range");
		return;
	}
	if (_column < 0 || _column >= m_columns) {
		OT_LOG_EA("Column index out of range");
		return;
	}

	if (m_items[_row][_column]) {
		OT_LOG_EA("At the given index another item was placed before. Deleting old item");
		delete m_items[_row][_column];
	}

	m_items[_row][_column] = _item;
}

void ot::GraphicsGridLayoutItemCfg::setRowStretch(int _row, int _stretch) {
	m_rowStretch[_row] = _stretch;
}

void ot::GraphicsGridLayoutItemCfg::setColumnStretch(int _column, int _stretch) {
	m_columnStretch[_column] = _stretch;
}

void ot::GraphicsGridLayoutItemCfg::clearAndResize(void) {
	for (const auto& c : m_items) {
		for (auto r : c) {
			if (r) delete r;
		}
	}

	m_items.clear();
	m_items.resize(m_rows);
	for (size_t r = 0; r < m_items.size(); r++) {
		m_items[r].resize(m_columns, nullptr);
	}
	m_rowStretch.resize(m_rows, 0);
	m_columnStretch.resize(m_columns, 0);
}

