// @otlicense

// OpenTwin header
#include "OTWidgets/Action.h"
#include "OTWidgets/ToolButton.h"

ot::ToolButton::ToolButton(QWidget* _parent) : QToolButton(_parent), m_action(nullptr) {
	m_action = new Action;
	this->ini();
}

ot::ToolButton::ToolButton(const QString& _text, QWidget* _parent)
	: QToolButton(_parent), m_action(nullptr) 
{
	m_action = new Action(_text);
	this->ini();
}

ot::ToolButton::ToolButton(const QIcon& _icon, const QString& _text, QWidget* _parent)
	: QToolButton(_parent), m_action(nullptr) 
{
	m_action = new Action(_icon, _text);
	this->ini();
}

ot::ToolButton::~ToolButton() {}

// #######################################################################################################

// Private member

void ot::ToolButton::ini(void) {
	this->setDefaultAction(m_action);
	this->setToolTip(QString());
	this->setWhatsThis(QString());
	this->setFocusPolicy(Qt::FocusPolicy::NoFocus);
	m_action->setToolTip(QString());
	m_action->setWhatsThis(QString());
	m_action->setMenuRole(QAction::MenuRole::NoRole);
	m_action->setIconVisibleInMenu(true);
}
