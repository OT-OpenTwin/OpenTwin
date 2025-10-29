// @otlicense

//! @file TableItemDelegate.h
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtWidgets/qstyleditemdelegate.h>

namespace ot {

	class Table;

	class OT_WIDGETS_API_EXPORT TableItemDelegate : public QStyledItemDelegate {
		OT_DECL_NODEFAULT(TableItemDelegate)
		OT_DECL_NOCOPY(TableItemDelegate)
	public:
		TableItemDelegate(Table* _table);
		virtual ~TableItemDelegate();

		virtual void paint(QPainter* _painter, const QStyleOptionViewItem& _option, const QModelIndex& _index) const override;

		virtual QSize sizeHint(const QStyleOptionViewItem& _option, const QModelIndex& _index) const override;

		virtual QWidget* createEditor(QWidget* _parent, const QStyleOptionViewItem& _option, const QModelIndex& _index) const override;

		virtual void setEditorData(QWidget* _editor, const QModelIndex& _index) const override;

		virtual void updateEditorGeometry(QWidget* _editor, const QStyleOptionViewItem& option, const QModelIndex& _index) const override;

	private:
		Table* m_table;

	};

}