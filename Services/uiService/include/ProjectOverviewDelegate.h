// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtWidgets/qstyleditemdelegate.h>

namespace ot {

	class ProjectOverviewDelegate : public QStyledItemDelegate {
		OT_DECL_NOCOPY(ProjectOverviewDelegate)
		OT_DECL_NOMOVE(ProjectOverviewDelegate)
	public:
		ProjectOverviewDelegate() = default;
		virtual ~ProjectOverviewDelegate() = default;

		virtual void paint(QPainter* _painter, const QStyleOptionViewItem& _option, const QModelIndex& _index) const override;

		virtual QSize sizeHint(const QStyleOptionViewItem& _option, const QModelIndex& _index) const override;

	private:

	};
}