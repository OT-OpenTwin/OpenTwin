//! @file GraphicsGridLayoutItemCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/GraphicsItemCfgFactory.h"
#include "OTGui/GraphicsGridLayoutItemCfg.h"

#define OT_JSON_MEMBER_Rows "Rows"
#define OT_JSON_MEMBER_Items "Items"
#define OT_JSON_MEMBER_Columns "Columns"
#define OT_JSON_MEMBER_RowStretch "RowStretch"
#define OT_JSON_MEMBER_ColumnStretch "ColumnStretch"

static ot::GraphicsItemCfgFactoryRegistrar<ot::GraphicsGridLayoutItemCfg> ellipseItemRegistrar(OT_FactoryKey_GraphicsGridLayoutItem);

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

ot::GraphicsItemCfg* ot::GraphicsGridLayoutItemCfg::createCopy(void) const {
	ot::GraphicsGridLayoutItemCfg* copy = new GraphicsGridLayoutItemCfg;
	this->setupData(copy);

	copy->m_rows = m_rows;
	copy->m_columns = m_columns;
	copy->m_rowStretch = m_rowStretch;
	copy->m_columnStretch = m_columnStretch;

	for (const std::vector<GraphicsItemCfg*>& r : m_items) {
		std::vector<GraphicsItemCfg*> row;
		for (GraphicsItemCfg* itm : r) {
			if (itm) {
				row.push_back(itm->createCopy());
			}
			else {
				row.push_back(nullptr);
			}
		}
		copy->m_items.push_back(row);
	}

	return copy;
}

void ot::GraphicsGridLayoutItemCfg::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	ot::GraphicsLayoutItemCfg::addToJsonObject(_object, _allocator);
	_object.AddMember(OT_JSON_MEMBER_Rows, m_rows, _allocator);
	_object.AddMember(OT_JSON_MEMBER_Columns, m_columns, _allocator);

	JsonArray rowStretchArr(m_rowStretch, _allocator);
	_object.AddMember(OT_JSON_MEMBER_RowStretch, rowStretchArr, _allocator);

	JsonArray columnStretchArr(m_columnStretch, _allocator);
	_object.AddMember(OT_JSON_MEMBER_ColumnStretch, columnStretchArr, _allocator);

	JsonArray itemArr;
	for (auto r : m_items) {
		JsonArray columnArr;
		for (auto c : r) {
			if (c) {
				JsonObject itemObj;
				c->addToJsonObject(itemObj, _allocator);
				columnArr.PushBack(itemObj, _allocator);
			}
			else {
				JsonNullValue itemObj;
				columnArr.PushBack(itemObj, _allocator);
			}
		}
		itemArr.PushBack(columnArr, _allocator);
	}
	_object.AddMember(OT_JSON_MEMBER_Items, itemArr, _allocator);
}

void ot::GraphicsGridLayoutItemCfg::setFromJsonObject(const ConstJsonObject& _object) {
	ot::GraphicsLayoutItemCfg::setFromJsonObject(_object);
	
	m_rows = json::getInt(_object, OT_JSON_MEMBER_Rows);
	m_columns = json::getInt(_object, OT_JSON_MEMBER_Columns);

	// Clear and resize the arrays
	this->clearAndResize();

	m_rowStretch = json::getIntVector(_object, OT_JSON_MEMBER_RowStretch);
	m_columnStretch = json::getIntVector(_object, OT_JSON_MEMBER_ColumnStretch);
	ConstJsonArray itemsArr = _object[OT_JSON_MEMBER_Items].GetArray();

	// Ensure correct array sizes
	if (itemsArr.Size() != m_rows) {
		OT_LOG_EA("Item array size mismatch");
		return;
	}

	// Items
	for (rapidjson::SizeType r = 0; r < itemsArr.Size(); r++) {
		ConstJsonArray columnArr = json::getArray(itemsArr, r);
		if (columnArr.Size() != m_columns) {
			OT_LOG_EA("Item column array size mismatch");
			return;
		}
		for (rapidjson::SizeType c = 0; c < columnArr.Size(); c++) {
			m_items[r][c] = nullptr;
			if (columnArr[c].IsNull()) {
				continue;
			}
			else if (columnArr[c].IsObject()) {
				GraphicsItemCfg* itm = nullptr;
				try {
					ConstJsonObject itemObj = json::getObject(columnArr, c);
					itm = GraphicsItemCfgFactory::instance().create(itemObj);
					if (!itm) {
						continue;
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

