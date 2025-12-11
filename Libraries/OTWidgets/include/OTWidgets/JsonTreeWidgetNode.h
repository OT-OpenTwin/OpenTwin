// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtCore/qjsonvalue.h>
#include <QtCore/qjsonarray.h>
#include <QtCore/qjsonobject.h>

// std header
#include <vector>

namespace ot {

    class OT_WIDGETS_API_EXPORT JsonTreeWidgetNode {
		OT_DECL_NOCOPY(JsonTreeWidgetNode)
		OT_DECL_NOMOVE(JsonTreeWidgetNode)
		OT_DECL_NODEFAULT(JsonTreeWidgetNode)
    public:
        JsonTreeWidgetNode(const QString& _key, const QJsonValue& _value, JsonTreeWidgetNode* _parent = nullptr);
        JsonTreeWidgetNode(const QString& _key, QJsonValue&& _value, JsonTreeWidgetNode* _parent = nullptr);
        ~JsonTreeWidgetNode();

        JsonTreeWidgetNode* getChild(int _row);

        int getChildCount();

        int getRow() const;

        void loadChildren();

        const QString& getKey() const { return m_key; };
		void setValue(const QJsonValue& _value) { m_value = _value; };
		void setValue(QJsonValue&& _value) { m_value = std::move(_value); };
		const QJsonValue& getValue() const { return m_value; };
		JsonTreeWidgetNode* getParent() const { return m_parent; };
		const std::vector<JsonTreeWidgetNode*>& getChildren() const { return m_children; };
		bool getChildrenLoaded() const { return m_childrenLoaded; };

		bool isContainer() const { return m_isContainer; };
		bool isContainerEnd() const { return m_isContainerEnd; };
        bool isEditProtected() const;

    private:
        bool m_isContainer;
        bool m_isContainerEnd;
        QString m_key;
        QJsonValue m_value;
        JsonTreeWidgetNode* m_parent;
        std::vector<JsonTreeWidgetNode*> m_children;
        bool m_childrenLoaded;
    };

}