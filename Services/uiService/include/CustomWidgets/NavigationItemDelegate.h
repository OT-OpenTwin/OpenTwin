// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/CoreTypes.h"

// Qt header
#include <QtWidgets/qstyleditemdelegate.h>

class NavigationItemDelegate : public QStyledItemDelegate {
	OT_DECL_DEFCOPY(NavigationItemDelegate)
	OT_DECL_DEFMOVE(NavigationItemDelegate)
public:
	NavigationItemDelegate() = default;
	virtual ~NavigationItemDelegate() = default;

	QWidget* createEditor(QWidget* _parent, const QStyleOptionViewItem& _option, const QModelIndex& _index) const override;

private:

};