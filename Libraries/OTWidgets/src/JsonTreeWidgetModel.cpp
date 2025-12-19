// @otlicense

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTWidgets/GlobalColorStyle.h"
#include "OTWidgets/JsonTreeWidgetModel.h"

ot::JsonTreeWidgetModel::JsonTreeWidgetModel(QObject* _parent)
    : QAbstractItemModel(_parent), m_rootNode(nullptr)
{
    m_rootNode = new JsonTreeWidgetNode("", JsonNullValue());
}

ot::JsonTreeWidgetModel::~JsonTreeWidgetModel() {
    if (m_rootNode) {
        delete m_rootNode;
        m_rootNode = nullptr;
	}
}

void ot::JsonTreeWidgetModel::setFromJsonDocument(const JsonDocument& _doc) {
    beginResetModel();
    
    if (m_rootNode) {
        delete m_rootNode;
		m_rootNode = nullptr;
    }

    if (!_doc.IsObject() && !_doc.IsArray()) {
		OT_LOG_E("Document is neither object nor array");
        m_rootNode = new JsonTreeWidgetNode("", JsonNullValue());
	}
    else {
        m_rootNode = new JsonTreeWidgetNode("", _doc);
    }
    
    endResetModel();
}

QModelIndex ot::JsonTreeWidgetModel::index(int _row, int _column, const QModelIndex& _parent) const {
    if (!hasIndex(_row, _column, _parent)) {
        return QModelIndex();
    }
    JsonTreeWidgetNode* parentNode = _parent.isValid() ? static_cast<JsonTreeWidgetNode*>(_parent.internalPointer()) : m_rootNode;
    JsonTreeWidgetNode* childNode = parentNode->getChild(_row);
    if (childNode) {
        return createIndex(_row, _column, childNode);
    }
    else {
        return QModelIndex();
    }
}

QModelIndex ot::JsonTreeWidgetModel::parent(const QModelIndex& _index) const {
    if (!_index.isValid()) {
        return QModelIndex();
    }
    JsonTreeWidgetNode* node = static_cast<JsonTreeWidgetNode*>(_index.internalPointer());
    if (!node->getParent() || node->getParent() == m_rootNode) {
        return QModelIndex();
    }
    else {
        return createIndex(node->getParent()->getRow(), 0, node->getParent());
    }
}

int ot::JsonTreeWidgetModel::rowCount(const QModelIndex& _parent) const {
    JsonTreeWidgetNode* parentNode = _parent.isValid() ? static_cast<JsonTreeWidgetNode*>(_parent.internalPointer()) : m_rootNode;
    return parentNode->getChildCount();
}

int ot::JsonTreeWidgetModel::columnCount(const QModelIndex& _parent) const {
	OT_UNUSED(_parent);
    return ColumnIndex::ColumnCount;
}

QVariant ot::JsonTreeWidgetModel::data(const QModelIndex& _index, int _role) const {
    if (!_index.isValid()) {
        return {};
    }
    JsonTreeWidgetNode* node = static_cast<JsonTreeWidgetNode*>(_index.internalPointer());

    if (_role == Qt::DisplayRole) {
        if (_index.column() == ColumnIndex::ColumnKey) {
            return node->getKey();
		}
        else if (_index.column() == ColumnIndex::ColumnValue) {
            return node->getValue();
        }
        else if (_index.column() == ColumnIndex::ColumnType) {
            if (!node->isContainer() && !node->isContainerEnd()) {
                return node->getTypeString();
            }
		}
        else {
            OT_LOG_E("Invalid column index requested");
        }
    }
    else if (_role == Qt::ForegroundRole) {
        const ColorStyle& style = GlobalColorStyle::instance().getCurrentStyle();
        if (_index.column() == ColumnIndex::ColumnKey) {
            if (node->isContainer() || node->isContainerEnd()) {
                return style.getValue(ColorStyleValueEntry::JsonWidgetBracket).toColor();
            }
            else if (node->getParent() && node->getParent()->getType() == JsonTreeWidgetNode::Object) {
                return style.getValue(ColorStyleValueEntry::JsonWidgetKey).toColor();
            }
        }
        else if (_index.column() == ColumnIndex::ColumnValue) {
            if (node->getType() == JsonTreeWidgetNode::String) {
				return style.getValue(ColorStyleValueEntry::JsonWidgetString).toColor();
            }
            else if (node->getType() == JsonTreeWidgetNode::Number) {
                return style.getValue(ColorStyleValueEntry::JsonWidgetNumber).toColor();
            }
            else if (node->getType() == JsonTreeWidgetNode::Bool) {
                return style.getValue(ColorStyleValueEntry::JsonWidgetBoolean).toColor();
            }
            else if (node->getType() == JsonTreeWidgetNode::Null) {
                return style.getValue(ColorStyleValueEntry::JsonWidgetNull).toColor();
			}
        }
    }
    return {};
}

QVariant ot::JsonTreeWidgetModel::headerData(int _section, Qt::Orientation _orientation, int _role) const {
    if (_role != Qt::DisplayRole) {
        return {};
    }
    else if (_orientation == Qt::Horizontal) {
        switch (_section) {
        case 0: return "Key";
        case 1: return "Value";
        case 2: return "Type";
        default:
            OT_LOG_E("Invalid section index requested");
            return {};
        }
    }
    else {
        return {};
    }
}

Qt::ItemFlags ot::JsonTreeWidgetModel::flags(const QModelIndex& _index) const {
    if (!_index.isValid()) {
        return Qt::NoItemFlags;
    }
    JsonTreeWidgetNode* node = static_cast<JsonTreeWidgetNode*>(_index.internalPointer());
    Qt::ItemFlags f = Qt::ItemIsSelectable | Qt::ItemIsEnabled;
    return f;
}

bool ot::JsonTreeWidgetModel::setData(const QModelIndex& _index, const QVariant& _value, int _role) {
    if (!_index.isValid() || _role != Qt::EditRole) {
        return false;
    }
    JsonTreeWidgetNode* node = static_cast<JsonTreeWidgetNode*>(_index.internalPointer());
    if (_index.column() == 1) {
       
    }
    return false;
}
