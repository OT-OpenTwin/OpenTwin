// @otlicense

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTWidgets/GlobalColorStyle.h"
#include "OTWidgets/JsonTreeWidgetModel.h"

ot::JsonTreeWidgetModel::JsonTreeWidgetModel(QObject* _parent)
    : QAbstractItemModel(_parent), m_rootNode(nullptr)
{
    m_rootNode = new JsonTreeWidgetNode("Document", QJsonValue::Object);
}

ot::JsonTreeWidgetModel::~JsonTreeWidgetModel() {
    if (m_rootNode) {
        delete m_rootNode;
        m_rootNode = nullptr;
	}
}

void ot::JsonTreeWidgetModel::setJson(const QJsonDocument& _doc) {
    beginResetModel();
    
    if (m_rootNode) {
        delete m_rootNode;
		m_rootNode = nullptr;
    }

    if (!_doc.isObject() && !_doc.isArray()) {
		OT_LOG_E("Document is neither object nor array");
        m_rootNode = new JsonTreeWidgetNode("Root", QJsonValue::Null);
	}
    else {
        m_rootNode = new JsonTreeWidgetNode("Root", _doc.isObject() ? QJsonValue(_doc.object()) : QJsonValue(_doc.array()));
    }
    
    endResetModel();
}

QJsonDocument ot::JsonTreeWidgetModel::toJsonDocument() const {
    if (!m_rootNode) {
        OT_LOG_W("No document set");
        return QJsonDocument();
	}

    if (m_rootNode->getValue().isObject()) {
        QJsonObject obj;
        addToJsonObject(obj, m_rootNode);
        return QJsonDocument(obj);
    }
    else if (m_rootNode->getValue().isArray()) {
        QJsonArray arr;
        addToJsonArray(arr, m_rootNode);
        return QJsonDocument(arr);
    }
    else {
        OT_LOG_W("Root node is neither object nor array");
        return QJsonDocument();
	}
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
    return 3;
}

QVariant ot::JsonTreeWidgetModel::data(const QModelIndex& _index, int _role) const {
    if (!_index.isValid()) {
        return {};
    }
    JsonTreeWidgetNode* node = static_cast<JsonTreeWidgetNode*>(_index.internalPointer());

    if (_role == Qt::DisplayRole) {
        if (_index.column() == 0) {
            return node->getKey();
		}
        else if (_index.column() == 1) {
            if (node->getValue().isObject()) {
                return "";
            }
            else if (node->getValue().isArray()) {
                return "";
            }
            else {
                return jsonValueToString(node->getValue());
            }
        }
        else if (_index.column() == 2) {
            if (!node->isContainer() && !node->isContainerEnd()) {
                switch (node->getValue().type()) {
                case QJsonValue::Null: return "Null";
                case QJsonValue::Bool: return "Boolean";
                case QJsonValue::Double: return "Number";
                case QJsonValue::String: return "String";
                case QJsonValue::Array: return "Array";
                case QJsonValue::Object: return "Object";
                default: return "Unknown";
                }
            }
		}
    }
    else if (_role == Qt::ForegroundRole) {
        const ColorStyle& style = GlobalColorStyle::instance().getCurrentStyle();
        if (_index.column() == 1) {
            if (node->getValue().isString()) {
				return style.getValue(ColorStyleValueEntry::JsonWidgetString).toColor();
            }
            else if (node->getValue().isDouble()) {
                return style.getValue(ColorStyleValueEntry::JsonWidgetNumber).toColor();
            }
            else if (node->getValue().isBool()) {
                return style.getValue(ColorStyleValueEntry::JsonWidgetBoolean).toColor();
            }
            else if (node->getValue().isNull()) {
                return style.getValue(ColorStyleValueEntry::JsonWidgetNull).toColor();
			}
        }
        else if (_index.column() == 0) {
            if (node->isContainer() || node->isContainerEnd()) {
				return style.getValue(ColorStyleValueEntry::JsonWidgetBracket).toColor();
            }
            else if (node->getParent() && node->getParent()->getValue().isObject()) {
				return style.getValue(ColorStyleValueEntry::JsonWidgetKey).toColor();
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
    if (!node->getValue().isObject() && !node->getValue().isArray() && _index.column() == 1) {
        f |= Qt::ItemIsEditable;
    }
    return f;
}

bool ot::JsonTreeWidgetModel::setData(const QModelIndex& _index, const QVariant& _value, int _role) {
    if (!_index.isValid() || _role != Qt::EditRole) {
        return false;
    }
    JsonTreeWidgetNode* node = static_cast<JsonTreeWidgetNode*>(_index.internalPointer());
    if (_index.column() == 1) {
        // basic type conversion
        if (node->getValue().isString()) {
            node->setValue(_value.toString());
        }
        else if (node->getValue().isDouble()) {
            node->setValue(_value.toDouble());
        }
        else if (node->getValue().isBool()) {
            node->setValue(_value.toBool());
        }
        else {
			OT_LOG_E("Unsupported type for editing");
			return false;
        }
        
        Q_EMIT dataChanged(_index, _index);
        return true;
    }
    return false;
}

void ot::JsonTreeWidgetModel::addToJsonObject(QJsonObject& _object, JsonTreeWidgetNode* _node) const {
	OTAssert(_node->getValue().isObject(), "Node is not an object");
    _node->loadChildren();
    const auto& children = _node->getChildren();
    
    if (_node->isContainer()) {
        for (const auto& child : children) {
			QString key = child->getKey();
            OTAssert(key.length() >= 3, "Invalid key size");
            key.removeFirst();
            key.removeLast();
            key.removeLast();
            if (child->getValue().type() == QJsonValue::Object) {
                QJsonObject childObject;
                addToJsonObject(childObject, child);
                _object.insert(key, childObject);
            }
            else if (child->getValue().type() == QJsonValue::Array) {
                QJsonArray childArray;
                addToJsonArray(childArray, child);
                _object.insert(key, childArray);
            }
            else {
                _object.insert(key, child->getValue());
			}
		}
    }
    else {
        // Find container
        for (const auto& child : children) {
            if (child->isContainerEnd()) {
                continue;
            }
            else if (child->isContainer()) {
                if (child->getValue().isObject()) {
                    addToJsonObject(_object, child);
                }
                else {
                    OT_LOG_E("Unexpected container type in object");
                }
            }
            else {
                OT_LOG_E("Unexpected object child");
            }
        }
    }
}

void ot::JsonTreeWidgetModel::addToJsonArray(QJsonArray& _array, JsonTreeWidgetNode* _node) const {
    OTAssert(_node->getValue().isArray(), "Node is not an array");
    _node->loadChildren();
    const auto& children = _node->getChildren();
    
    if (_node->isContainer()) {
        for (const auto& child : children) {
            if (child->getValue().type() == QJsonValue::Object) {
                QJsonObject childObject;
                addToJsonObject(childObject, child);
                _array.append(childObject);
            }
            else if (child->getValue().type() == QJsonValue::Array) {
                QJsonArray childArray;
                addToJsonArray(childArray, child);
                _array.append(childArray);
            }
            else {
                _array.append(child->getValue());
            }
        }
    }
    else {
        // Find container
        for (const auto& child : children) {
            if (child->isContainerEnd()) {
                continue;
            }
            else if (child->isContainer()) {
                if (child->getValue().isArray()) {
                    addToJsonArray(_array, child);
                }
                else {
                    OT_LOG_E("Unexpected container type in array");
                }
            }
            else {
                OT_LOG_E("Unexpected array child");
            }
        }
	}
}

QString ot::JsonTreeWidgetModel::jsonValueToString(const QJsonValue& _value) {
    switch (_value.type()) {
    case QJsonValue::Null: return "null";
    case QJsonValue::Bool: return _value.toBool() ? "true" : "false";
    case QJsonValue::Double: 
    {
        double d = _value.toDouble();
        qint64 i = static_cast<qint64>(d);

        // Check if value is an integer
        if (d == static_cast<double>(i)) {
            return QString::number(_value.toInt());
        }
        else {
            // Non-integer or too large for 64-bit integer
            return QString::number(d, 'g', 15);
        }
        
    }
    case QJsonValue::String: return _value.toString();
    default: return ""; // Object or Array handled elsewhere
    }
}