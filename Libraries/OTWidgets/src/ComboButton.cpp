//! @file ComboButton.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/ComboButton.h"

// Qt header
#include <QtWidgets/qmenu.h>

ot::ComboButton::ComboButton(QWidget* _parent)
	: PushButton(_parent)
{
	this->setObjectName("OT_ComboButton");

	m_menu = new QMenu(this);

	this->ini();
}

ot::ComboButton::ComboButton(const QString& _text, QWidget* _parent) 
	: PushButton(_text, _parent)
{
	this->setObjectName("OT_ComboButton");

	m_menu = new QMenu(this);
	m_menu->addAction(_text);

	this->ini();
}

ot::ComboButton::ComboButton(const QString& _text, const QStringList& _items, QWidget* _parent)
	: PushButton(_text, _parent)
{
	this->setObjectName("OT_ComboButton");

	m_menu = new QMenu(this);
	for (const QString& itm : _items) {
		m_menu->addAction(itm);
	}

	this->ini();
}

ot::ComboButton::~ComboButton() {
	delete m_menu;
}

void ot::ComboButton::setItems(const QStringList& _items) {
	m_menu->clear();
	for (const QString& itm : _items) {
		m_menu->addAction(itm);
	}
}

void ot::ComboButton::mousePressEvent(QMouseEvent* _event) {
	PushButton::mousePressEvent(_event);
}

void ot::ComboButton::slotActionTriggered(QAction* _action) {
	this->setText(_action->text());
	Q_EMIT textChanged();
}

void ot::ComboButton::ini(void) {
	this->setMenu(m_menu);
	this->connect(m_menu, &QMenu::triggered, this, &ComboButton::slotActionTriggered);
	this->connect(m_menu, &QMenu::aboutToShow, this, &ComboButton::setSelectedProperty);
	this->connect(m_menu, &QMenu::aboutToHide, this, &ComboButton::unsetSelectedProperty);
}
