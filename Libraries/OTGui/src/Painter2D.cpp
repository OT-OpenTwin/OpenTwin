//! @file Painter2D.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCore/OTAssert.h"
#include "OTCore/SimpleFactory.h"
#include "OTGui/Painter2D.h"

void ot::Painter2D::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember(OT_SimpleFactoryJsonKey, JsonString(this->simpleFactoryObjectKey(), _allocator), _allocator);
}

void ot::Painter2D::setFromJsonObject(const ConstJsonObject& _object) {

}

ot::Painter2D* ot::Painter2D::createCopy(void) const {
	ot::JsonDocument doc;
	this->addToJsonObject(doc, doc.GetAllocator());

	Painter2D* newPainter = SimpleFactory::instance().createType<Painter2D>(doc.GetConstObject());
	OTAssertNullptr(newPainter);
	newPainter->setFromJsonObject(doc.GetConstObject());
	return newPainter;
}