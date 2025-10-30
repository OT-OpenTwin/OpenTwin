// @otlicense

// OpenTwin header
#include "OTWidgets/CheckBox.h"

// Qt header
#include <QtWidgets/qstyle.h>

ot::CheckBox::CheckBox(QWidget* _parent) : QCheckBox(_parent) {}

ot::CheckBox::CheckBox(const QString& _text, QWidget* _parent) : QCheckBox(_text, _parent) {}

ot::CheckBox::CheckBox(Qt::CheckState _state, const QString& _text, QWidget* _parent) : QCheckBox(_text, _parent) 
{
	this->setCheckState(_state);
}
