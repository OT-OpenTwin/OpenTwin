//! @file PropertyGridItemDelegate.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtWidgets/qstyleditemdelegate.h>

namespace ot {

	class TreeWidget;

	class PropertyGridItemDelegate : public QStyledItemDelegate {
		OT_DECL_NOCOPY(PropertyGridItemDelegate)
	public:
		PropertyGridItemDelegate(TreeWidget* _tree);
		virtual ~PropertyGridItemDelegate();

		virtual void paint(QPainter* _painter, const QStyleOptionViewItem& _option, const QModelIndex& _index) const override;

	private:
		TreeWidget* m_tree;

		PropertyGridItemDelegate() = delete;
	};

}