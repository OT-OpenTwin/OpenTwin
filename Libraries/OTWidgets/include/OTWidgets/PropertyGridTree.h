//! @file PropertyGridTree.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/TreeWidget.h"

namespace ot {

	class OT_WIDGETS_API_EXPORT PropertyGridTree : public TreeWidget {
		Q_OBJECT
		OT_DECL_NOCOPY(PropertyGridTree)
	public:
		PropertyGridTree();
		virtual ~PropertyGridTree();

	protected:
		virtual void mousePressEvent(QMouseEvent* _event) override;

		virtual void showEvent(QShowEvent* _event);

		virtual void resizeEvent(QResizeEvent* _event);

		virtual void drawRow(QPainter* _painter, const QStyleOptionViewItem& _options, const QModelIndex& _index) const override;

	private:
		bool m_wasShown;
	};
}