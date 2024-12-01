//! @file PropertyInputStringList.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/PropertyStringList.h"
#include "OTWidgets/ComboBox.h"
#include "OTWidgets/LineEdit.h"
#include "OTWidgets/PropertyInputStringList.h"
#include "OTWidgets/PropertyInputFactoryRegistrar.h"

// Qt header
#include <QtWidgets/qmenu.h>
#include <QtWidgets/qaction.h>
#include <QtWidgets/qlineedit.h>
#include <QtWidgets/qabstractitemview.h>

static ot::PropertyInputFactoryRegistrar<ot::PropertyInputStringList> propertyInputStringListRegistrar(ot::PropertyStringList::propertyTypeString());

ot::PropertyInputStringList::PropertyInputStringList()
{
	m_comboBox = new ComboBox;
	m_comboBox->setEditable(true);
	m_comboBox->setFocusPolicy(Qt::NoFocus);

	LineEdit* customLineEdit = new LineEdit;
	customLineEdit->setReadOnly(true);
	m_comboBox->setLineEdit(customLineEdit);
	this->connect(m_comboBox, &ComboBox::currentTextChanged, this, qOverload<>(&PropertyInput::slotValueChanged));
	this->connect(customLineEdit, &LineEdit::leftMouseButtonPressed, m_comboBox, &ComboBox::togglePopup);
}

ot::PropertyInputStringList::~PropertyInputStringList() {
	delete m_comboBox;
}

void ot::PropertyInputStringList::addPropertyInputValueToJson(ot::JsonValue& _object, const char* _memberNameValue, ot::JsonAllocator& _allocator) {
	_object.AddMember(JsonString(_memberNameValue, _allocator), JsonString(m_comboBox->currentText().toStdString(), _allocator), _allocator);
}

QVariant ot::PropertyInputStringList::getCurrentValue(void) const {
	return QVariant(m_comboBox->currentText());
}

QWidget* ot::PropertyInputStringList::getQWidget(void) {
	return m_comboBox;
}

ot::Property* ot::PropertyInputStringList::createPropertyConfiguration(void) const {
	ot::PropertyStringList* newProperty = new ot::PropertyStringList(this->data());

	std::list<std::string> lst;
	for (int i = 0; i < m_comboBox->count(); i++) {
		lst.push_back(m_comboBox->itemText(i).toStdString());
	}
	newProperty->setList(lst);
	newProperty->setCurrent(m_comboBox->currentText().toStdString());

	return newProperty;
}

bool ot::PropertyInputStringList::setupFromConfiguration(const Property* _configuration) {
	if (!PropertyInput::setupFromConfiguration(_configuration)) return false;
	const PropertyStringList* actualProperty = dynamic_cast<const PropertyStringList*>(_configuration);
	if (!actualProperty) {
		OT_LOG_E("Property cast failed");
		return false;
	}

	QStringList lst;
	for (const std::string& itm : actualProperty->getList()) {
		lst.push_back(QString::fromStdString(itm));
	}
	
	m_comboBox->blockSignals(true);

	m_comboBox->addItems(lst);
	if (this->data().getPropertyFlags() & Property::HasMultipleValues) {
		m_comboBox->setCurrentText("...");
	}
	else {
		m_comboBox->setCurrentText(QString::fromStdString(actualProperty->getCurrent()));
	}
	m_comboBox->setEnabled(!(this->data().getPropertyFlags() & Property::IsReadOnly));

	m_comboBox->blockSignals(false);

	return true;
}

void ot::PropertyInputStringList::focusPropertyInput(void) {
	m_comboBox->setFocus();
}

void ot::PropertyInputStringList::setCurrentText(const QString& _text) {
	m_comboBox->setCurrentText(_text);
}

QString ot::PropertyInputStringList::getCurrentText(void) const {
	return m_comboBox->currentText();
}

QStringList ot::PropertyInputStringList::getPossibleSelection(void) const {
	QStringList lst;
	for (int i = 0; i < m_comboBox->count(); i++) {
		lst.append(m_comboBox->itemText(i));
	}
	return lst;
}
