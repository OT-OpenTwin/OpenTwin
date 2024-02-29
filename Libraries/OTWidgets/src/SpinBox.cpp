//! @file SpinBox.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/OTAssert.h"
#include "OTWidgets/SpinBox.h"

ot::SpinBox::SpinBox(QWidget* _parent) : QSpinBox(_parent) {}

ot::SpinBox::SpinBox(int _min, int _max, int _value, QWidget* _parent) : QSpinBox(_parent) {
	OTAssert(_value >= _min && _value <= _max && _max >= _min, "Invalid setup");
	this->setRange(_min, _max);
	this->setValue(_value);
}

ot::SpinBox::~SpinBox() {}