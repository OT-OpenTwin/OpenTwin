// @otlicense

// OpenTwin header
#include "OTWidgets/Widgets/GroupBox.h"

ot::GroupBox::GroupBox(QWidget* _parent)
	: QGroupBox(_parent)
{}

ot::GroupBox::GroupBox(const QString & _title, QWidget * _parent)
	: QGroupBox(_title, _parent)
{}
