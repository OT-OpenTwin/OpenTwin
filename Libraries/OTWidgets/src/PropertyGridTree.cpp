//! @file PropertyGridTree.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/PropertyGridTree.h"

// Qt header
#include <QtGui/qevent.h>

ot::PropertyGridTree::PropertyGridTree() : m_wasShown(false) {

}

ot::PropertyGridTree::~PropertyGridTree() {

}

void ot::PropertyGridTree::mousePressEvent(QMouseEvent* _event) {
	QModelIndex index = indexAt(_event->pos());
	bool last_state = isExpanded(index);

	TreeWidget::mousePressEvent(_event);

	if (index.isValid() && last_state == isExpanded(index) && this->itemFromIndex(index) && this->itemFromIndex(index)->childCount() > 0) {
		this->setExpanded(index, !last_state);
	}

}

void ot::PropertyGridTree::showEvent(QShowEvent* _event) {
	if (!m_wasShown) {
		this->setColumnWidth(0, this->width() / 2);
		m_wasShown = true;
	}
	TreeWidget::showEvent(_event);
}

void ot::PropertyGridTree::resizeEvent(QResizeEvent* _event) {
	TreeWidget::resizeEvent(_event);
}

void ot::PropertyGridTree::drawRow(QPainter* _painter, const QStyleOptionViewItem& _options, const QModelIndex& _index) const {
	TreeWidget::drawRow(_painter, _options, _index);
}
