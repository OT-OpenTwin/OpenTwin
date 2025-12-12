// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetBase.h"
#include "OTWidgets/JsonTreeWidgetNode.h"
#include "OTWidgets/JsonTreeWidgetModel.h"
#include "OTWidgets/JsonTreeWidgetFilterModel.h"

// Qt header
#include <QtWidgets/qtreeview.h>

namespace ot {

    class OT_WIDGETS_API_EXPORT JsonTreeWidget : public QTreeView {
        Q_OBJECT
        OT_DECL_NOCOPY(JsonTreeWidget)
        OT_DECL_NOMOVE(JsonTreeWidget)
        OT_DECL_NODEFAULT(JsonTreeWidget)
    public:
        JsonTreeWidget(QWidget* _parent);
		virtual ~JsonTreeWidget();

        void setJsonDocument(const QJsonDocument& _doc);
		QJsonDocument toJsonDocument() const;

        void setReadOnly(bool _readOnly) { m_readOnly = _readOnly; };
		bool isReadOnly() const { return m_readOnly; };

    public Q_SLOTS:
		void filterItems(const QString& _filterText);

    protected:
		virtual bool edit(const QModelIndex& _index, QAbstractItemView::EditTrigger _trigger, QEvent* _event) override;

    private Q_SLOTS:
		void slotItemExpanded(const QModelIndex& _index);

    private:
        JsonTreeWidgetModel* m_model;
        JsonTreeWidgetFilterModel* m_filterModel;
        bool m_readOnly;
    };

}
