//! @file TreeItemDelegate.cpp
//! @author Alexander Kuester (alexk95)
//! @date June 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/TreeItemDelegate.h"

QWidget* ot::TreeItemDelegate::createEditor(QWidget* _parent, const QStyleOptionViewItem& _option, const QModelIndex& _index) const {
	LineEdit* newEdit = new LineEdit(_parent);
	newEdit->setObjectName("OT_TreeItemEdit");
	return newEdit;
}
