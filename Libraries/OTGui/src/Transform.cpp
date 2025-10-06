//! @file Transform.cpp
//! @author Alexander Kuester (alexk95)
//! @date June 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTGui/Transform.h"

ot::Transform::Transform()
	: m_rotation(0.), m_flipState(FlipState::NoFlip)
{}

ot::Transform::Transform(double _rotationAngle, const FlipStateFlags& _flipState) 
	: m_rotation(_rotationAngle), m_flipState(_flipState)
{}

ot::Transform::Transform(const Transform& _other) 
	: m_rotation(_other.m_rotation), m_flipState(_other.m_flipState)
{

}

ot::Transform::~Transform() {}

ot::Transform& ot::Transform::operator = (const Transform& _other) {
	if (this != &_other) {
		m_rotation = _other.m_rotation;
		m_flipState = _other.m_flipState;
	}
	return *this;
}

void ot::Transform::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("Rotation", m_rotation, _allocator);
	JsonArray flipStateArray;
	if (m_flipState & FlipState::FlipHorizontally) flipStateArray.PushBack(JsonString("Horizontal", _allocator), _allocator);
	if (m_flipState & FlipState::FlipVertically) flipStateArray.PushBack(JsonString("Vertical", _allocator), _allocator);
	_object.AddMember("Flip", flipStateArray, _allocator);
}

void ot::Transform::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_rotation = json::getDouble(_object, "Rotation");
	m_flipState = NoFlip;
	std::list<std::string> flipStateArray = json::getStringList(_object, "Flip");
	for (const std::string& flipStateEntry : flipStateArray) {
		if (flipStateEntry == "Horizontal") m_flipState.setFlag(FlipState::FlipHorizontally);
		else if (flipStateEntry == "Vertical") m_flipState.setFlag(FlipState::FlipVertically);
	}
}

