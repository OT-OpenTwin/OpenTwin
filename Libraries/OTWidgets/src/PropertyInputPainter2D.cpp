// @otlicense

//! @file PropertyInputPainter2D.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTGui/Painter2D.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/PropertyPainter2D.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/Painter2DEditButton.h"
#include "OTWidgets/PropertyInputPainter2D.h"
#include "OTWidgets/PropertyInputFactoryRegistrar.h"

static ot::PropertyInputFactoryRegistrar<ot::PropertyInputPainter2D> propertyInputPainter2DRegistrar(ot::PropertyPainter2D::propertyTypeString());

ot::PropertyInputPainter2D::PropertyInputPainter2D()
{
	m_button = new Painter2DEditButton;
	m_button->getPushButton()->setObjectName("OT_PropertyInputPainter2D_Button");

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

const QWidget* ot::PropertyInputPainter2D::getQWidget(void) const {
	return m_button->getQWidget();
}

ot::Property* ot::PropertyInputPainter2D::createPropertyConfiguration(void) const {
	ot::PropertyPainter2D* newProperty = new ot::PropertyPainter2D(this->data());

	const Painter2D* p = m_button->getPainter();
	if (p) newProperty->setPainter(p->createCopy());
	else newProperty->setPainter(new FillPainter2D);

	newProperty->setFilter(m_button->getFilter());

	return newProperty;
}

bool ot::PropertyInputPainter2D::setupFromConfiguration(const Property* _configuration) {
	if (!PropertyInput::setupFromConfiguration(_configuration)) return false;
	const PropertyPainter2D* actualProperty = dynamic_cast<const PropertyPainter2D*>(_configuration);
	if (!actualProperty) {
		OT_LOG_E("Property cast failed");
		return false;
	}

	m_button->blockSignals(true);

	m_button->setPainter(actualProperty->getPainter());
	m_button->setFilter(actualProperty->getFilter());
	m_button->getPushButton()->setToolTip(QString::fromStdString(this->data().getPropertyTip()));
	if (this->data().getPropertyFlags() & Property::HasMultipleValues) {
		m_button->getPushButton()->setText("...");
	}
	m_button->getPushButton()->setEnabled(!(this->data().getPropertyFlags() & Property::IsReadOnly));

	m_button->blockSignals(false);

	return true;
}

void ot::PropertyInputPainter2D::focusPropertyInput(void) {
	m_button->getPushButton()->setFocus();
}

const ot::Painter2D* ot::PropertyInputPainter2D::getPainter(void) const {
	return m_button->getPainter();
}