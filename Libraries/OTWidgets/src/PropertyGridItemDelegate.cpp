//! @file PropertyGridItemDelegate.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/PropertyGridItemDelegate.h"

// Qt header
#include <QtWidgets/qapplication.h>

ot::PropertyGridItemDelegate::PropertyGridItemDelegate(QObject* _parent)
	: QStyledItemDelegate(_parent), m_firstTabSpace(20)
{

}

void ot::PropertyGridItemDelegate::paint(QPainter* _painter, const QStyleOptionViewItem& _option, const QModelIndex& _index) const {
	QStyleOptionViewItem opt(_option);
	this->initStyleOption(&opt, _index);

	if (_index.column() == 0) {
		opt.rect.adjust(m_firstTabSpace, 0, 0, 0);
	}
	QStyledItemDelegate::paint(_painter, opt, _index);
	if (_index.column() == 0) {
		QStyleOptionViewItem branch;
		branch.rect = QRect(0, opt.rect.y(), opt.rect.height(), opt.rect.height());
		branch.state = _option.state;
		const QWidget* widget = _option.widget;
		QStyle* style = widget ? widget->style() : QApplication::style();
		style->drawPrimitive(QStyle::PE_IndicatorBranch, &branch, _painter, widget);
	}

	QStyledItemDelegate::paint(_painter, opt, _index);
}
