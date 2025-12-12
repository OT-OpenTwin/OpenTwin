// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtCore/qsortfilterproxymodel.h>

namespace ot {
	
	class OT_WIDGETS_API_EXPORT JsonTreeWidgetFilterModel : public QSortFilterProxyModel {
		OT_DECL_NOCOPY(JsonTreeWidgetFilterModel)
		OT_DECL_NOMOVE(JsonTreeWidgetFilterModel)
		OT_DECL_NODEFAULT(JsonTreeWidgetFilterModel)
    public:
        JsonTreeWidgetFilterModel(QObject* _parent);

        void setFilterText(const QString& _text);

    protected:
        bool filterAcceptsRow(int _sourceRow, const QModelIndex& _sourceParent) const override;

    private:
        QString m_filterText;

        bool rowMatches(const QModelIndex& _index) const;

        bool hasMatchingParent(QModelIndex _parent) const;

        bool hasMatchingChildren(const QModelIndex& _parent) const;
	};

}