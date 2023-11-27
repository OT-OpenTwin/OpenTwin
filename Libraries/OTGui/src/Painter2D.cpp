//! @file Painter2D.cpp
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/Painter2D.h"
#include "OTCore/OTAssert.h"
#include "OTCore/Logger.h"

void ot::Painter2D::addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const {
	_object.AddMember(OT_SimpleFactoryJsonKey, JsonString(this->simpleFactoryObjectKey(), _allocator), _allocator);
}

void ot::Painter2D::setFromJsonObject(const ConstJsonObject& _object) {

}