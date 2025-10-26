// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtWidgets/qheaderview.h>

namespace ot {

	class ProjectOverviewHeader : public QHeaderView {
		Q_OBJECT
		OT_DECL_NOCOPY(ProjectOverviewHeader)
		OT_DECL_NOMOVE(ProjectOverviewHeader)
		OT_DECL_NODEFAULT(ProjectOverviewHeader)
	public:
		ProjectOverviewHeader(QWidget* _parent);
		virtual ~ProjectOverviewHeader();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Overrides

		virtual int sizeHintForColumn(int _column) const override;

	protected:
		virtual void paintSection(QPainter* _painter, const QRect& _rect, int _logicalIndex) const override;
		virtual QSize sectionSizeFromContents(int _logicalIndex) const override;

	private:
		const QSize c_buttonSize;
		const QSize c_buttonPadding;
		int m_hoveredFilter;
		int m_pressedFilter;
	};

}