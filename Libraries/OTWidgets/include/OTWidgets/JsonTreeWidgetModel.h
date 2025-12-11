// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/JsonTreeWidgetNode.h"

// Qt header
#include <QtCore/qvariant.h>
#include <QtCore/qjsondocument.h>
#include <QtCore/qabstractitemmodel>

namespace ot {

    class OT_WIDGETS_API_EXPORT JsonTreeWidgetModel : public QAbstractItemModel {
        Q_OBJECT
		OT_DECL_NOCOPY(JsonTreeWidgetModel)
		OT_DECL_NOMOVE(JsonTreeWidgetModel)
		OT_DECL_NODEFAULT(JsonTreeWidgetModel)
    public:
        JsonTreeWidgetModel(QObject* _parent);
        virtual ~JsonTreeWidgetModel();

        void setJson(const QJsonDocument& _doc);
		QJsonDocument toJsonDocument() const;

        // Required overrides
        QModelIndex index(int _row, int _column, const QModelIndex& _parent = QModelIndex()) const override;

        QModelIndex parent(const QModelIndex& _index) const override;

        int rowCount(const QModelIndex& _parent = QModelIndex()) const override;

        int columnCount(const QModelIndex& _parent = QModelIndex()) const override;

        QVariant data(const QModelIndex& _index, int _role = Qt::DisplayRole) const override;

        QVariant headerData(int _section, Qt::Orientation _orientation, int _role) const override;

        Qt::ItemFlags flags(const QModelIndex& _index) const override;

        bool setData(const QModelIndex& _index, const QVariant& _value, int _role = Qt::EditRole) override;

    private:
        JsonTreeWidgetNode* m_rootNode;

		void addToJsonObject(QJsonObject& _object, JsonTreeWidgetNode* _node) const;
		void addToJsonArray(QJsonArray& _array, JsonTreeWidgetNode* _node) const;

        static QString jsonValueToString(const QJsonValue& _value);
    };

}
