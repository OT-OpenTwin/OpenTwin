// @otlicense

// OpenTwin header
#include "OTWidgets/ComboBox.h"

// Qt header
#include <QtGui/qevent.h>

ot::ComboBox::ComboBox(QWidget* _parent) 
	: QComboBox(_parent), m_popupVisible(false)
{

}

void ot::ComboBox::showPopup(void) {
	m_popupVisible = true;
	QComboBox::showPopup();
}

void ot::ComboBox::hidePopup(void) {
	m_popupVisible = false;
	QComboBox::hidePopup();
}

void ot::ComboBox::keyPressEvent(QKeyEvent* _event) {
	QComboBox::keyPressEvent(_event);

	if (_event->key() == Qt::Key_Return) {
		Q_EMIT returnPressed();
	}
}

void ot::ComboBox::togglePopup(void) {
	if (m_popupVisible) {
		this->hidePopup();
	}
	else {
		this->showPopup();
	}
}
