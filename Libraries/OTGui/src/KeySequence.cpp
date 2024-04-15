//! @file KeySequence.cpp
//! @author Alexander Kuester (alexk95)
//! @date April 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTGui/KeySequence.h"

ot::KeySequence::KeySequence() {}

ot::KeySequence::KeySequence(BasicKey _key) {
	m_keys.push_back(_key);
}

ot::KeySequence::KeySequence(BasicKey _key, BasicKey _key2) {
	m_keys.push_back(_key);
	m_keys.push_back(_key2);
}

ot::KeySequence::KeySequence(BasicKey _key, BasicKey _key2, BasicKey _key3) {
	m_keys.push_back(_key);
	m_keys.push_back(_key2);
	m_keys.push_back(_key3);
}

ot::KeySequence::KeySequence(BasicKey _key, BasicKey _key2, BasicKey _key3, BasicKey _key4) {
	m_keys.push_back(_key);
	m_keys.push_back(_key2);
	m_keys.push_back(_key3);
	m_keys.push_back(_key4);
}

ot::KeySequence::KeySequence(const std::list<BasicKey>& _keys) : m_keys(_keys) {}

ot::KeySequence::~KeySequence() {}

ot::KeySequence::operator std::string(void) const {
	std::string str;
	for (BasicKey k : m_keys) {
		if (!str.empty()) str.append("+");
		str.append(toString(k));
	}
	return str;
}