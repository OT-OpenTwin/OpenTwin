//! @file TreeItemDelegate.h
//! @author Alexander Kuester (alexk95)
//! @date June 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtWidgets/qstyleditemdelegate.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT TreeItemDelegate : public QStyledItemDelegate {
		OT_DECL_NOCOPY(TreeItemDelegate)
		OT_DECL_DEFMOVE(TreeItemDelegate)
	public:
		TreeItemDelegate() {};
		virtual ~TreeItemDelegate() = default;

		virtual QWidget* createEditor(QWidget* _parent, const QStyleOptionViewItem& _option, const QModelIndex& _index) const override;

	private:

	};

}