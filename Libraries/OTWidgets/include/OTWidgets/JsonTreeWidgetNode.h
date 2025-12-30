// @otlicense
// File: JsonTreeWidgetNode.h
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

#pragma once

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtCore/qstring.h>

// std header
#include <vector>

namespace ot {

    class OT_WIDGETS_API_EXPORT JsonTreeWidgetNode {
		OT_DECL_NOCOPY(JsonTreeWidgetNode)
		OT_DECL_NOMOVE(JsonTreeWidgetNode)
		OT_DECL_NODEFAULT(JsonTreeWidgetNode)
    public:
        enum ValueType {
            Null,
            Bool,
            String,
            Number,
            Object,
            Array
        };

        enum NodeFlag : uint64_t {
            NoFlags      = 0 << 0,
            Container    = 1 << 0,
			ContainerEnd = 1 << 1
        };
		typedef ot::Flags<NodeFlag> NodeFlags;

        JsonTreeWidgetNode(const QString& _key, const JsonValue& _jsonValue, const NodeFlags& _flags = NodeFlag::NoFlags, JsonTreeWidgetNode* _parent = nullptr);
        ~JsonTreeWidgetNode();

        JsonTreeWidgetNode* getChild(int _row);

        int getChildCount();

        int getRow() const;

        const QString& getKey() const { return m_key; };
		const QString& getValue() const { return m_value; };
		ValueType getType() const { return m_type; };
		const QString& getTypeString() const { return m_typeString; };
		JsonTreeWidgetNode* getParent() const { return m_parent; };
		const std::vector<JsonTreeWidgetNode*>& getChildren() const { return m_children; };
		
		bool isContainer() const { return m_flags.has(Container); };
		bool isContainerEnd() const { return m_flags.has(ContainerEnd); };
        bool isEditProtected() const;

    private:
        NodeFlags m_flags;
        QString m_key;
        QString m_value;
		ValueType m_type;
        QString m_typeString;
        JsonTreeWidgetNode* m_parent;
        std::vector<JsonTreeWidgetNode*> m_children;
    };

}

OT_ADD_FLAG_FUNCTIONS(ot::JsonTreeWidgetNode::NodeFlag, ot::JsonTreeWidgetNode::NodeFlags)