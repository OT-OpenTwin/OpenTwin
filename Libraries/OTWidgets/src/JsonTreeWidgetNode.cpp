// @otlicense
// File: JsonTreeWidgetNode.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTWidgets/JsonTreeWidgetNode.h"

// Qt header
#include <QtCore/qjsonarray.h>
#include <QtCore/qjsonobject.h>

ot::JsonTreeWidgetNode::JsonTreeWidgetNode(const QString& _key, const JsonValue& _jsonValue, const NodeFlags& _flags, JsonTreeWidgetNode* _parent)
	: m_key(_key), m_parent(_parent), m_flags(_flags), m_type(Null)
{
    if (_jsonValue.IsNull()) {
		m_value = "null";
		m_type = Null;
		m_typeString = "Null";
    }
    else if (_jsonValue.IsArray()) {
        if (m_flags.has(Container)) {
            // Load array children
            for (JsonSizeType i = 0; i < _jsonValue.Size(); i++) {
                m_children.push_back(new JsonTreeWidgetNode("[" + QString::number(i) + "]", _jsonValue[i], NodeFlag::NoFlags, this));
			}
        }
        else if (m_flags.has(ContainerEnd)) {
            m_value = "";
            m_type = Array;
            m_typeString = "";
		}
        else {
            m_value = "";
            m_type = Array;
            m_typeString = "Array";
            m_children.push_back(new JsonTreeWidgetNode("[", _jsonValue, NodeFlag::Container, this));
			m_children.push_back(new JsonTreeWidgetNode("]", JsonArray(), NodeFlag::ContainerEnd, this));
        }
    }
    else if (_jsonValue.IsObject()) {
        if (m_flags.has(Container)) {
            // Load object children
            for (JsonMemberIterator it = _jsonValue.MemberBegin(); it != _jsonValue.MemberEnd(); it++) {
                m_children.push_back(new JsonTreeWidgetNode("\"" + QString::fromUtf8(it->name.GetString(), it->name.GetStringLength()) + "\":", it->value, NodeFlag::NoFlags, this));
            }
            m_value = "";
            m_type = Object;
            m_typeString = "";
        }
        else if (m_flags.has(ContainerEnd)) {
            m_value = "";
            m_type = Object;
            m_typeString = "";
        }
        else {
            m_value = "";
            m_type = Object;
            m_typeString = "Object";
            m_children.push_back(new JsonTreeWidgetNode("{", _jsonValue, NodeFlag::Container, this));
            m_children.push_back(new JsonTreeWidgetNode("}", JsonObject(), NodeFlag::ContainerEnd, this));
        }
    }
    else if (_jsonValue.IsBool()) {
		m_value = _jsonValue.GetBool() ? "true" : "false";
		m_type = Bool;
		m_typeString = "Bool";
    }
    else if (_jsonValue.IsUint()) {
        m_value = QString::number(_jsonValue.GetUint());
        m_type = Number;
        m_typeString = "UInt";
    }
    else if (_jsonValue.IsInt()) {
        m_value = QString::number(_jsonValue.GetInt());
		m_type = Number;
		m_typeString = "Int";
    }
    else if (_jsonValue.IsUint64()) {
        m_value = QString::number(_jsonValue.GetUint64());
        m_type = Number;
        m_typeString = "UInt64";
    }
    else if (_jsonValue.IsInt64()) {
        m_value = QString::number(_jsonValue.GetInt64());
		m_type = Number;
		m_typeString = "Int64";
    }
    else if (_jsonValue.IsDouble()) {
		m_value = QString::number(_jsonValue.GetDouble(), 'g', 15);
		m_type = Number;
		m_typeString = "Double";
    }
    else if (_jsonValue.IsString()) {
		m_value = QString::fromUtf8(_jsonValue.GetString(), qsizetype(_jsonValue.GetStringLength()));
		m_type = String;
		m_typeString = "String";
    }
    else {
        OT_LOG_E("Unknown JSON value type encountered");
    }
}

ot::JsonTreeWidgetNode::~JsonTreeWidgetNode() {
    for (ot::JsonTreeWidgetNode* child : m_children) {
        delete child;
    }
}

ot::JsonTreeWidgetNode* ot::JsonTreeWidgetNode::getChild(int _row) {
    if (_row < 0 || _row >= m_children.size()) {
        return nullptr;
    }
    else {
        return m_children[_row];
    }
}

int ot::JsonTreeWidgetNode::getChildCount() {
    return static_cast<int>(m_children.size());
}

int ot::JsonTreeWidgetNode::getRow() const {
    if (!m_parent) {
        return 0;
    }
    for (int i = 0; i < m_parent->m_children.size(); i++) {
        if (m_parent->m_children[i] == this) {
            return i;
        }
    }
    return -1;
}

bool ot::JsonTreeWidgetNode::isEditProtected() const {
    return m_flags.hasAny(NodeFlag::Container | NodeFlag::ContainerEnd) || m_type == Object || m_type == Array;
}
