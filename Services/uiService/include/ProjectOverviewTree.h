// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/TreeWidget.h"

namespace ot {
	
	class ProjectOverviewTree : public TreeWidget {
		OT_DECL_NOCOPY(ProjectOverviewTree)
		OT_DECL_NOMOVE(ProjectOverviewTree)
	public:
		ProjectOverviewTree(QWidget* _parent = (QWidget*)nullptr);
		virtual ~ProjectOverviewTree() = default;
		
		virtual void drawRow(QPainter* _painter, const QStyleOptionViewItem& _option, const QModelIndex& _index) const override;

	};

}
