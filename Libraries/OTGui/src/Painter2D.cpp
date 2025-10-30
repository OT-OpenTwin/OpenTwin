// @otlicense

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTGui/Painter2D.h"
#include "OTGui/Painter2DFactory.h"

void ot::Painter2D::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember(OT_JSON_MEMBER_Painter2DType, JsonString(this->getFactoryKey(), _allocator), _allocator);
}

void ot::Painter2D::setFromJsonObject(const ConstJsonObject& _object) {

}

ot::Painter2D* ot::Painter2D::createCopy(void) const {
	ot::JsonDocument doc;
	this->addToJsonObject(doc, doc.GetAllocator());

	return Painter2DFactory::create(doc.getConstObject());
}