//! @file TreeItemDelegate.cpp
//! @author Alexander Kuester (alexk95)
//! @date June 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/TreeItemDelegate.h"

// Qt header
#include <QtWidgets/qtreewidget.h>

ot::TreeItemDelegate::TreeItemDelegate(QTreeWidget* _tree) : m_tree(_tree) {
	OTAssertNullptr(m_tree);
	m_tree->setItemDelegate(this);
}

ot::TreeItemDelegate::~TreeItemDelegate() {
	m_tree->setItemDelegate(nullptr);
	m_tree = nullptr;
}

QWidget* ot::TreeItemDelegate::createEditor(QWidget* _parent, const QStyleOptionViewItem& _option, const QModelIndex& _index) const {
	LineEdit* newEdit = new LineEdit(_parent);
	newEdit->setObjectName("OT_TreeItemEdit");
	return newEdit;
}
