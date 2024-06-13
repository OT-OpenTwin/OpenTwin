//! @file PropertyInputStringList.cpp
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/PropertyStringList.h"
#include "OTWidgets/ComboButton.h"
#include "OTWidgets/PropertyInputStringList.h"
#include "OTWidgets/PropertyInputFactoryRegistrar.h"

// Qt header
#include <QtWidgets/qmenu.h>
#include <QtWidgets/qaction.h>

static ot::PropertyInputFactoryRegistrar<ot::PropertyInputStringList> propertyInputStringListRegistrar(OT_PROPERTY_TYPE_StringList);

ot::PropertyInputStringList::PropertyInputStringList()
{
	m_comboButton = new ComboButton;
	this->connect(m_comboButton, &ComboButton::textChanged, this, qOverload<>(&PropertyInput::slotValueChanged));
}

ot::PropertyInputStringList::~PropertyInputStringList() {
	delete m_comboButton;
}

void ot::PropertyInputStringList::addPropertyInputValueToJson(ot::JsonValue& _object, const char* _memberNameValue, ot::JsonAllocator& _allocator) {
	_object.AddMember(JsonString(_memberNameValue, _allocator), JsonString(m_comboButton->text().toStdString(), _allocator), _allocator);
}

QVariant ot::PropertyInputStringList::getCurrentValue(void) const {
	return QVariant(m_comboButton->text());
}

QWidget* ot::PropertyInputStringList::getQWidget(void) {
	return m_comboButton;
}

ot::Property* ot::PropertyInputStringList::createPropertyConfiguration(void) const {
	ot::PropertyStringList* newProperty = new ot::PropertyStringList(this->data());

	std::list<std::string> lst;
	for (const QAction* a : m_comboButton->menu()->actions()) {
		lst.push_back(a->text().toStdString());
	}
	newProperty->setList(lst);
	newProperty->setCurrent(m_comboButton->text().toStdString());

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
	
	m_comboButton->blockSignals(true);

	m_comboButton->setItems(lst);
	if (this->data().getPropertyFlags() & Property::HasMultipleValues) m_comboButton->setText("...");
	else m_comboButton->setText(QString::fromStdString(actualProperty->getCurrent()));
	m_comboButton->setEnabled(!(this->data().getPropertyFlags() & Property::IsReadOnly));

	m_comboButton->blockSignals(false);

	return true;
}

void ot::PropertyInputStringList::setCurrentText(const QString& _text) {
	m_comboButton->setText(_text);
}

QString ot::PropertyInputStringList::getCurrentText(void) const {
	return m_comboButton->text();
}

QStringList ot::PropertyInputStringList::getPossibleSelection(void) const {
	QStringList lst;
	for (const QAction* a : m_comboButton->menu()->actions()) {
		lst.append(a->text());
	}
	return lst;
}