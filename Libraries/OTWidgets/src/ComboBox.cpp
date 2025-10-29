// @otlicense

//! @file ComboBox.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/ComboBox.h"

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

void ot::ComboBox::togglePopup(void) {
	if (m_popupVisible) {
		this->hidePopup();
	}
	else {
		this->showPopup();
	}
}
