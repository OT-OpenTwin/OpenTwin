// @otlicense

// OpenTwin header
#include "OTWidgets/JsonTreeWidgetNode.h"

// Qt header
#include <QtCore/qjsonarray.h>
#include <QtCore/qjsonobject.h>

ot::JsonTreeWidgetNode::JsonTreeWidgetNode(const QString& _key, const QJsonValue& _value, JsonTreeWidgetNode* _parent)
	: m_key(_key), m_value(_value), m_parent(_parent), m_childrenLoaded(false), m_isContainer(false), m_isContainerEnd(false)
{}

ot::JsonTreeWidgetNode::JsonTreeWidgetNode(const QString& _key, QJsonValue&& _value, JsonTreeWidgetNode* _parent)
	: m_key(_key), m_value(std::move(_value)), m_parent(_parent), m_childrenLoaded(false), m_isContainer(false), m_isContainerEnd(false)
{}

ot::JsonTreeWidgetNode::~JsonTreeWidgetNode() {
    for (ot::JsonTreeWidgetNode* child : m_children) {
        delete child;
    }
}

ot::JsonTreeWidgetNode* ot::JsonTreeWidgetNode::getChild(int _row) {
    loadChildren();
    if (_row < 0 || _row >= m_children.size()) {
        return nullptr;
    }
    else {
        return m_children[_row];
    }
}

int ot::JsonTreeWidgetNode::getChildCount() {
    loadChildren();
    return static_cast<int>(m_children.size());
}

int ot::JsonTreeWidgetNode::getRow() const {
    if (!m_parent) {
        return 0;
    }
    for (int i = 0; i < m_parent->m_children.size(); ++i) {
        if (m_parent->m_children[i] == this) {
            return i;
        }
    }
    return 0;
}

void ot::JsonTreeWidgetNode::loadChildren() {
    if (m_childrenLoaded) {
        return;
    }
    m_childrenLoaded = true;

    if (m_value.isObject()) {
        if (m_isContainer) {
            QJsonObject obj = m_value.toObject();
            for (auto it = obj.constBegin(); it != obj.constEnd(); ++it) {
                m_children.push_back(new JsonTreeWidgetNode("\"" + it.key() + "\":", it.value(), this));
            }
        }
        else {
            JsonTreeWidgetNode* openingNode = new JsonTreeWidgetNode("{", std::move(m_value), this);
			m_value = QJsonValue(QJsonObject());
			openingNode->m_isContainer = true;
            m_children.push_back(openingNode);

			JsonTreeWidgetNode* closingNode = new JsonTreeWidgetNode("}", QJsonValue(""), this);
            closingNode->m_isContainerEnd = true;
            m_children.push_back(closingNode);
        }
    }
    else if (m_value.isArray()) {
        if (m_isContainer) {
            QJsonArray arr = m_value.toArray();
            for (int i = 0; i < arr.size(); ++i) {
                m_children.push_back(new JsonTreeWidgetNode("[" + QString::number(i) + "]", arr[i], this));
            }
        }
        else {
            JsonTreeWidgetNode* openingNode = new JsonTreeWidgetNode("[", std::move(m_value), this);
            m_value = QJsonValue(QJsonArray());
            openingNode->m_isContainer = true;
            m_children.push_back(openingNode);

            JsonTreeWidgetNode* closingNode = new JsonTreeWidgetNode("]", QJsonValue(""), this);
            closingNode->m_isContainerEnd = true;
            m_children.push_back(closingNode);
        }
    }
}

bool ot::JsonTreeWidgetNode::isEditProtected() const {
	return m_isContainer || m_isContainerEnd || m_value.isArray() || m_value.isObject();
}
