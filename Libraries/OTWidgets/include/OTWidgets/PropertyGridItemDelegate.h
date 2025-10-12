//! @file PropertyGridItemDelegate.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtWidgets/qstyleditemdelegate.h>

namespace ot {

	class TreeWidget;

	class OT_WIDGETS_API_EXPORT PropertyGridItemDelegate : public QStyledItemDelegate {
		OT_DECL_NOCOPY(PropertyGridItemDelegate)
		OT_DECL_NOMOVE(PropertyGridItemDelegate)
	public:
		PropertyGridItemDelegate(TreeWidget* _tree);
		virtual ~PropertyGridItemDelegate();

		virtual void paint(QPainter* _painter, const QStyleOptionViewItem& _option, const QModelIndex& _index) const override;

	private:
		TreeWidget* m_tree;

		PropertyGridItemDelegate() = delete;
	};

}