// @otlicense

// OpenTwin header
#include "CustomWidgets/NavigationItemDelegate.h"
#include "OTWidgets/Syntax/EntityNameValidator.h"
#include "OTWidgets/Widgets/LineEdit.h"

QWidget* NavigationItemDelegate::createEditor(QWidget* _parent, const QStyleOptionViewItem& _option, const QModelIndex& _index) const
{
	ot::LineEdit* edit = new ot::LineEdit(_parent);
	edit->setValidator(new ot::EntityNameValidator);
	return edit;
}
