// @otlicense

//! @file PropertyGridItemDelegate.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTWidgets/TreeWidget.h"
#include "OTGui/ColorStyleTypes.h"
#include "OTWidgets/PropertyGridItem.h"
#include "OTWidgets/GlobalColorStyle.h"
#include "OTWidgets/PropertyGridGroup.h"
#include "OTWidgets/PropertyGridItemDelegate.h"

// Qt header
#include <QtGui/qpainter.h>

ot::PropertyGridItemDelegate::PropertyGridItemDelegate(TreeWidget* _tree) 
	: QStyledItemDelegate(_tree), m_tree(_tree)
{
	OTAssertNullptr(m_tree);
	m_tree->setItemDelegate(this);
}

ot::PropertyGridItemDelegate::~PropertyGridItemDelegate() {
	m_tree->setItemDelegate(nullptr);
	m_tree = nullptr;
}

void ot::PropertyGridItemDelegate::paint(QPainter* _painter, const QStyleOptionViewItem& _option, const QModelIndex& _index) const {
	PropertyGridItem* itm = dynamic_cast<PropertyGridItem*>(m_tree->itemFromIndex(_index));
	PropertyGridGroup* grp = dynamic_cast<PropertyGridGroup*>(m_tree->itemFromIndex(_index));

	if (itm) {
		_painter->fillRect(_option.rect, GlobalColorStyle::instance().getCurrentStyle().getValue(ColorStyleValueEntry::WidgetBackground).toBrush());
	}
	else if (grp) {
		_painter->fillRect(_option.rect, GlobalColorStyle::instance().getCurrentStyle().getValue(ColorStyleValueEntry::TitleBackground).toBrush());
	}
	else {
		OT_LOG_E("Unknown property grid entry");
	}

	QStyledItemDelegate::paint(_painter, _option, _index);
}