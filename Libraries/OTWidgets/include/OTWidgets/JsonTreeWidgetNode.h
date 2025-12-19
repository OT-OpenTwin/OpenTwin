// @otlicense

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