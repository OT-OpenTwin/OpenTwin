//! @file DoubleSpinBox.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/OTAssert.h"
#include "OTWidgets/DoubleSpinBox.h"

ot::DoubleSpinBox::DoubleSpinBox(QWidget* _parent) : QDoubleSpinBox(_parent) {}

ot::DoubleSpinBox::DoubleSpinBox(double _min, double _max, double _value, int _precision, QWidget* _parent) : QDoubleSpinBox(_parent) {
	OTAssert(_value >= _min && _value <= _max && _max >= _min, "Invalid setup");
	this->setRange(_min, _max);
	this->setDecimals(_precision);
	this->setValue(_value);
}

ot::DoubleSpinBox::~DoubleSpinBox() {}