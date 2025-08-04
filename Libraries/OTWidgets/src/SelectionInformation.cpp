//! @file SelectionInformation.cpp
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/SelectionInformation.h"

ot::SelectionInformation::SelectionInformation() {}

ot::SelectionInformation::SelectionInformation(const SelectionInformation& _other) :
	m_uids(_other.m_uids)
{}

ot::SelectionInformation::SelectionInformation(SelectionInformation&& _other) noexcept :
	m_uids(std::move(_other.m_uids))
{
	_other.m_uids.clear();
}

ot::SelectionInformation::~SelectionInformation() {}

ot::SelectionInformation& ot::SelectionInformation::operator=(const SelectionInformation& _other) {
	if (this != &_other) {
		m_uids = _other.m_uids;
	}

	return *this;
}

ot::SelectionInformation& ot::SelectionInformation::operator=(SelectionInformation&& _other) noexcept {
	if (this != &_other) {
		m_uids = std::move(_other.m_uids);
		_other.m_uids.clear();
	}

	return *this;
}

void ot::SelectionInformation::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	_object.AddMember("UIDs", JsonArray(m_uids, _allocator), _allocator);
}

void ot::SelectionInformation::setFromJsonObject(const ot::ConstJsonObject& _object) {
	m_uids = json::getUInt64List(_object, "UIDs");
}

void ot::SelectionInformation::removeSelectedNavigationItem(UID _uid) {
	auto it = std::find(m_uids.begin(), m_uids.end(), _uid);
	if (it != m_uids.end()) {
		m_uids.erase(it);
	}
}
