// @otlicense

// OpenTwin header
#include "OTWidgets/Action.h"

ot::Action::Action(QObject* _parent)
	: QAction(_parent)
{

}

ot::Action::Action(const QString& _text, QObject* _parent) 
	: QAction(_text, _parent)
{

}

ot::Action::Action(const QIcon& _icon, const QString& _text, QObject* _parent) 
	: QAction(_text, _parent)
{
	this->setIcon(_icon);
}

ot::Action::~Action() {}