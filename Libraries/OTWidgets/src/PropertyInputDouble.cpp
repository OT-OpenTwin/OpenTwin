//! @file PropertyInputDouble.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCore/StringHelper.h"
#include "OTCore/PropertyDouble.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/DoubleSpinBox.h"
#include "OTWidgets/PropertyInputDouble.h"
#include "OTWidgets/PropertyInputFactoryRegistrar.h"

static ot::PropertyInputFactoryRegistrar<ot::PropertyInputDouble> propertyInputDoubleRegistrar(OT_PROPERTY_TYPE_Double);

#define OT_PROPERTY_DOUBLE_MULTIPLEVALUESCHAR "."
#define OT_PROPERTY_DOUBLE_MULTIPLEVALUESTEXT OT_PROPERTY_DOUBLE_MULTIPLEVALUESCHAR OT_PROPERTY_DOUBLE_MULTIPLEVALUESCHAR OT_PROPERTY_DOUBLE_MULTIPLEVALUESCHAR

ot::PropertyInputDouble::PropertyInputDouble()
	: m_lineEdit(nullptr)
{
	m_spinBox = new DoubleSpinBox;
	this->connect(m_spinBox, &QDoubleSpinBox::valueChanged, this, &PropertyInputDouble::lclValueChanged);
}

ot::PropertyInputDouble::~PropertyInputDouble() {
	if (m_spinBox) delete m_spinBox;
	m_spinBox = nullptr;
	if (m_lineEdit) delete m_lineEdit;
	m_lineEdit = nullptr;
}

void ot::PropertyInputDouble::addPropertyInputValueToJson(ot::JsonValue& _object, const char* _memberNameValue, ot::JsonAllocator& _allocator) {
	if (m_spinBox) {
		_object.AddMember(JsonString(_memberNameValue, _allocator), JsonValue(m_spinBox->value()), _allocator);
	}
	else {
		OTAssertNullptr(m_lineEdit);
		bool failed = false;
		double n = stringToNumber<double>(m_lineEdit->text().toStdString(), failed);
		if (failed) {
			OT_LOG_W("Invalid number format. Defaulting");
			n = 0.;
		}
		_object.AddMember(JsonString(_memberNameValue, _allocator), JsonValue(n), _allocator);
	}
	
}

QVariant ot::PropertyInputDouble::getCurrentValue(void) const {
	if (m_spinBox) {
		return QVariant(m_spinBox->value());
	}
	else {
		OTAssertNullptr(m_lineEdit);
		bool failed = false;
		double n = stringToNumber<double>(m_lineEdit->text().toStdString(), failed);
		if (failed) {
			OT_LOG_W("Invalid number format. Defaulting");
			n = 0.;
		}
		return QVariant(n);
	}
}

QWidget* ot::PropertyInputDouble::getQWidget(void) {
	if (m_spinBox) {
		return m_spinBox;
	}
	else {
		OTAssertNullptr(m_lineEdit);
		return m_lineEdit;
	}
}

void ot::PropertyInputDouble::setValue(double _value) {
	if (m_spinBox) {
		m_spinBox->setValue(_value);
	}
	else {
		OTAssertNullptr(m_lineEdit);
		m_lineEdit->setText(QString::number(_value));
	}
}

double ot::PropertyInputDouble::getValue(void) const {
	if (m_spinBox) {
		return m_spinBox->value();
	}
	else {
		OTAssertNullptr(m_lineEdit);
		bool failed = false;
		double n = stringToNumber<double>(m_lineEdit->text().toStdString(), failed);
		if (failed) {
			OT_LOG_W("Invalid number format. Defaulting");
			n = 0.;
		}
		return n;
	}
}

bool ot::PropertyInputDouble::hasInputError(void) const {
	if (m_spinBox) {
		if (m_spinBox->text() == OT_PROPERTY_DOUBLE_MULTIPLEVALUESTEXT) return true;
		else return false;
	}
	else {
		OTAssertNullptr(m_lineEdit);
		bool failed = false;
		stringToNumber<double>(m_lineEdit->text().toStdString(), failed);
		return failed;
	}
}

void ot::PropertyInputDouble::lclValueChanged(int) {
	OTAssertNullptr(m_spinBox);
	m_spinBox->setSpecialValueText("");
	PropertyInput::slotValueChanged();
}

void ot::PropertyInputDouble::lclTextChanged(void) {
	OTAssertNullptr(m_lineEdit);
	if (this->data().propertyFlags() & Property::HasMultipleValues) {
		QString str = m_lineEdit->text();
		str.remove(OT_PROPERTY_DOUBLE_MULTIPLEVALUESCHAR);
		if (str.isEmpty()) str = OT_PROPERTY_DOUBLE_MULTIPLEVALUESCHAR;
		this->data().setPropertyFlag(Property::HasMultipleValues, false);

		m_lineEdit->blockSignals(true);
		m_lineEdit->setText(str);
		m_lineEdit->blockSignals(false);
	}

	if (this->hasInputError()) {
		m_lineEdit->setInputErrorStateProperty();
	}
	else {
		m_lineEdit->unsetInputErrorStateProperty();
	}
}

void ot::PropertyInputDouble::lclEditingFinishedChanged(void) {
	OTAssertNullptr(m_lineEdit);
	if (this->hasInputError()) return;
	PropertyInput::slotValueChanged();
}

ot::Property* ot::PropertyInputDouble::createPropertyConfiguration(void) const {
	ot::PropertyDouble* newProperty = new ot::PropertyDouble(this->data());

	if (m_spinBox) {
		newProperty->setMin(m_spinBox->minimum());
		newProperty->setMax(m_spinBox->maximum());
		
	}
	newProperty->setValue(this->getValue());

	return newProperty;
}

bool ot::PropertyInputDouble::setupFromConfiguration(const Property* _configuration) {
	if (!PropertyInput::setupFromConfiguration(_configuration)) return false;
	const PropertyDouble* actualProperty = dynamic_cast<const PropertyDouble*>(_configuration);
	if (!actualProperty) {
		OT_LOG_E("Property cast failed");
		return false;
	}

	if (this->data().propertyFlags() & Property::AllowCustomValues) {
		if (m_spinBox) delete m_spinBox;
		m_spinBox = nullptr;
		if (!m_lineEdit) m_lineEdit = new LineEdit;
		this->connect(m_lineEdit, &LineEdit::textChanged, this, &PropertyInputDouble::lclTextChanged);
		this->connect(m_lineEdit, &LineEdit::editingFinished, this, &PropertyInputDouble::lclEditingFinishedChanged);
	}

	if (m_spinBox) {
		m_spinBox->blockSignals(true);

		m_spinBox->setDecimals(actualProperty->precision());
		m_spinBox->setToolTip(QString::fromStdString(this->data().propertyTip()));
		m_spinBox->setRange(actualProperty->min(), actualProperty->max());
		m_spinBox->setValue(actualProperty->value());
		if (this->data().propertyFlags() & Property::HasMultipleValues) {
			m_spinBox->setSpecialValueText(OT_PROPERTY_DOUBLE_MULTIPLEVALUESTEXT);
		}

		m_spinBox->blockSignals(false);
	}
	else {
		OTAssertNullptr(m_lineEdit);
		m_lineEdit->blockSignals(true);

		m_lineEdit->setToolTip(QString::fromStdString(this->data().propertyTip()));
		m_lineEdit->setText(QString::number(actualProperty->value()));
		if (this->data().propertyFlags() & Property::HasMultipleValues) {
			m_lineEdit->setText(OT_PROPERTY_DOUBLE_MULTIPLEVALUESTEXT);
		}

		m_lineEdit->blockSignals(false);
	}
	

	return true;
}