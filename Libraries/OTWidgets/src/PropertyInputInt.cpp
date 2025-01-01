//! @file PropertyInputInt.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCore/String.h"
#include "OTGui/PropertyInt.h"
#include "OTWidgets/SpinBox.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/PropertyInputInt.h"
#include "OTWidgets/PropertyInputFactoryRegistrar.h"

static ot::PropertyInputFactoryRegistrar<ot::PropertyInputInt> propertyInputIntRegistrar(ot::PropertyInt::propertyTypeString());

#define OT_PROPERTY_INT_MULTIPLEVALUESCHAR "."
#define OT_PROPERTY_INT_MULTIPLEVALUESTEXT OT_PROPERTY_INT_MULTIPLEVALUESCHAR OT_PROPERTY_INT_MULTIPLEVALUESCHAR OT_PROPERTY_INT_MULTIPLEVALUESCHAR

ot::PropertyInputInt::PropertyInputInt()
	: m_lineEdit(nullptr), m_min(std::numeric_limits<int>::lowest()), m_max(std::numeric_limits<int>::max())
{
	m_spinBox = new SpinBox;
	this->connect(m_spinBox, &SpinBox::valueChangeCompleted, this, &PropertyInputInt::lclValueChanged);
}

ot::PropertyInputInt::~PropertyInputInt() {
	if (m_spinBox) delete m_spinBox;
	m_spinBox = nullptr;
	if (m_lineEdit) delete m_lineEdit;
	m_lineEdit = nullptr;
}

void ot::PropertyInputInt::addPropertyInputValueToJson(ot::JsonValue& _object, const char* _memberNameValue, ot::JsonAllocator& _allocator) {
	if (m_spinBox) {
		_object.AddMember(JsonString(_memberNameValue, _allocator), JsonValue(m_spinBox->value()), _allocator);
	}
	else {
		OTAssertNullptr(m_lineEdit);
		bool failed = false;
		int n = String::toNumber<int>(m_lineEdit->text().toStdString(), failed);
		if (failed) {
			OT_LOG_W("Invalid number format. Defaulting");
			n = 0;
		}
		_object.AddMember(JsonString(_memberNameValue, _allocator), JsonValue(n), _allocator);
	}

}

QVariant ot::PropertyInputInt::getCurrentValue(void) const {
	if (m_spinBox) {
		return QVariant(m_spinBox->value());
	}
	else {
		OTAssertNullptr(m_lineEdit);
		bool failed = false;
		int n = String::toNumber<int>(m_lineEdit->text().toStdString(), failed);
		if (failed) {
			OT_LOG_W("Invalid number format. Defaulting");
			n = 0;
		}
		return QVariant(n);
	}
}

QWidget* ot::PropertyInputInt::getQWidget(void) {
	if (m_spinBox) {
		return m_spinBox;
	}
	else {
		OTAssertNullptr(m_lineEdit);
		return m_lineEdit;
	}
}

const QWidget* ot::PropertyInputInt::getQWidget(void) const {
	if (m_spinBox) {
		return m_spinBox;
	}
	else {
		OTAssertNullptr(m_lineEdit);
		return m_lineEdit;
	}
}

void ot::PropertyInputInt::setValue(int _value) {
	if (m_spinBox) {
		m_spinBox->setValue(_value);
	}
	else {
		OTAssertNullptr(m_lineEdit);
		m_lineEdit->setText(QString::number(_value));
	}
}

int ot::PropertyInputInt::getValue(void) const {
	if (m_spinBox) {
		return m_spinBox->value();
	}
	else {
		OTAssertNullptr(m_lineEdit);
		bool failed = false;
		int n = String::toNumber<int>(m_lineEdit->text().toStdString(), failed);
		if (failed) {
			OT_LOG_W("Invalid number format. Defaulting");
			n = 0;
		}
		return n;
	}
}

bool ot::PropertyInputInt::hasInputError(void) const {
	if (m_spinBox) {
		if (m_spinBox->text() == OT_PROPERTY_INT_MULTIPLEVALUESTEXT) return true;
		else return false;
	}
	else {
		OTAssertNullptr(m_lineEdit);
		bool failed = false;
		String::toNumber<int>(m_lineEdit->text().toStdString(), failed);
		return failed;
	}
}

void ot::PropertyInputInt::lclValueChanged(int) {
	OTAssertNullptr(m_spinBox);
	m_spinBox->setSpecialValueText("");
	OTAssert(this->getValue() >= m_min && this->getValue() <= m_max, "Value out of range");
	PropertyInput::slotValueChanged();
}

void ot::PropertyInputInt::lclTextChanged(void) {
	OTAssertNullptr(m_lineEdit);
	if (this->data().getPropertyFlags() & Property::HasMultipleValues) {
		QString str = m_lineEdit->text();

		// Check for no changes and still multiple values
		if (str == OT_PROPERTY_INT_MULTIPLEVALUESTEXT) return;

		str.remove(OT_PROPERTY_INT_MULTIPLEVALUESCHAR);
		if (str.isEmpty()) str = OT_PROPERTY_INT_MULTIPLEVALUESCHAR;
		this->data().setPropertyFlag(Property::HasMultipleValues, false);

		m_lineEdit->blockSignals(true);
		m_lineEdit->setText(str);
		m_lineEdit->blockSignals(false);

		this->lclEditingFinishedChanged();
	}

	if (this->hasInputError()) {
		m_lineEdit->setInputErrorStateProperty();
	}
	else {
		m_lineEdit->unsetInputErrorStateProperty();
	}
}

void ot::PropertyInputInt::lclEditingFinishedChanged(void) {
	OTAssertNullptr(m_lineEdit);
	if (this->hasInputError()) return;

	int val = this->getValue();
	if (val < m_min) {
		m_lineEdit->blockSignals(true);
		m_lineEdit->setText(QString::number(m_min));
		m_lineEdit->blockSignals(false);
	}
	if (val > m_max) {
		m_lineEdit->blockSignals(true);
		m_lineEdit->setText(QString::number(m_max));
		m_lineEdit->blockSignals(false);
	}

	this->slotValueChanged();
}

ot::Property* ot::PropertyInputInt::createPropertyConfiguration(void) const {
	ot::PropertyInt* newProperty = new ot::PropertyInt(this->data());
	newProperty->setMin(m_min);
	newProperty->setMax(m_max);
	newProperty->setValue(this->getValue());

	return newProperty;
}

bool ot::PropertyInputInt::setupFromConfiguration(const Property* _configuration) {
	if (!PropertyInput::setupFromConfiguration(_configuration)) return false;
	const PropertyInt* actualProperty = dynamic_cast<const PropertyInt*>(_configuration);
	if (!actualProperty) {
		OT_LOG_E("Property cast failed");
		return false;
	}

	m_min = actualProperty->getMin();
	m_max = actualProperty->getMax();

	if (this->data().getPropertyFlags() & Property::AllowCustomValues) {
		if (m_spinBox) delete m_spinBox;
		m_spinBox = nullptr;
		if (!m_lineEdit) m_lineEdit = new LineEdit;
		this->connect(m_lineEdit, &LineEdit::textChanged, this, &PropertyInputInt::lclTextChanged);
		this->connect(m_lineEdit, &LineEdit::editingFinished, this, &PropertyInputInt::lclEditingFinishedChanged);
	}
	else if (m_spinBox == nullptr) {
		m_spinBox = new SpinBox;
		this->connect(m_spinBox, &SpinBox::valueChangeCompleted, this, &PropertyInputInt::lclValueChanged);
	}

	if (m_spinBox) {
		m_spinBox->blockSignals(true);

		m_spinBox->setToolTip(QString::fromStdString(this->data().getPropertyTip()));
		m_spinBox->setRange(actualProperty->getMin(), actualProperty->getMax());
		m_spinBox->setValue(actualProperty->getValue());
		if (this->data().getPropertyFlags() & Property::HasMultipleValues) {
			m_spinBox->setSpecialValueText(OT_PROPERTY_INT_MULTIPLEVALUESTEXT);
		}
		m_spinBox->setReadOnly(this->data().getPropertyFlags() & Property::IsReadOnly);

		m_spinBox->blockSignals(false);
	}
	else {
		OTAssertNullptr(m_lineEdit);
		m_lineEdit->blockSignals(true);

		m_lineEdit->setToolTip(QString::fromStdString(this->data().getPropertyTip()));
		m_lineEdit->setText(QString::number(actualProperty->getValue()));
		if (this->data().getPropertyFlags() & Property::HasMultipleValues) {
			m_lineEdit->setText(OT_PROPERTY_INT_MULTIPLEVALUESTEXT);
		}
		m_lineEdit->setReadOnly(this->data().getPropertyFlags() & Property::IsReadOnly);

		m_lineEdit->blockSignals(false);
	}


	return true;
}

void ot::PropertyInputInt::focusPropertyInput(void) {
	if (m_spinBox) {
		m_spinBox->setFocus();
	}
	else if (m_lineEdit) {
		m_lineEdit->setFocus();
	}
}