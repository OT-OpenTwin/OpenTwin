//! @file PropertyInputPainter2D.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/Painter2D.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/PropertyPainter2D.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/Painter2DEditButton.h"
#include "OTWidgets/PropertyInputPainter2D.h"

ot::PropertyInputPainter2D::PropertyInputPainter2D(const PropertyPainter2D* _property)
	: PropertyInput(_property)
{
	m_button = new Painter2DEditButton(_property->getPainter());
	m_button->getButton()->setToolTip(QString::fromStdString(_property->propertyTip()));
	if (_property->propertyFlags() & Property::HasMultipleValues) {
		m_button->getButton()->setText("...");
	}
	this->connect(m_button, &Painter2DEditButton::painter2DChanged, this, qOverload<>(&PropertyInput::slotValueChanged));
}

ot::PropertyInputPainter2D::~PropertyInputPainter2D() {
	delete m_button;
}

void ot::PropertyInputPainter2D::addPropertyInputValueToJson(ot::JsonValue& _object, const char* _memberNameValue, ot::JsonAllocator& _allocator) {
	JsonObject painterObj;
	m_button->getPainter()->addToJsonObject(painterObj, _allocator);
	_object.AddMember(JsonString(_memberNameValue, _allocator), painterObj, _allocator);
}

QVariant ot::PropertyInputPainter2D::getCurrentValue(void) const {
	JsonDocument doc;
	m_button->getPainter()->addToJsonObject(doc, doc.GetAllocator());

	return QVariant(QByteArray::fromStdString(doc.toJson()));
}

QWidget* ot::PropertyInputPainter2D::getQWidget(void) {
	return m_button->getQWidget();
}

ot::Property* ot::PropertyInputPainter2D::createPropertyConfiguration(void) const {
	ot::PropertyPainter2D* newProperty = new ot::PropertyPainter2D;
	newProperty->setPropertyName(this->propertyName());
	newProperty->setPropertyTitle(this->propertyTitle().toStdString());
	newProperty->setPropertyTip(this->propertyTip().toStdString());
	newProperty->setPropertyFlags(this->propertyFlags());

	const Painter2D* p = m_button->getPainter();
	if (p) newProperty->setPainter(p->createCopy());
	else newProperty->setPainter(new FillPainter2D);

	return newProperty;
}