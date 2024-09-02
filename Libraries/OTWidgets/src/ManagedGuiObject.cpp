//! @file ManagedGuiObject.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/ManagedGuiObject.h"

ot::ManagedGuiObject::ManagedGuiObject()
	: m_isEnabled(true), m_disabledCounter(0)
{

}

ot::ManagedGuiObject::~ManagedGuiObject() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// State management

void ot::ManagedGuiObject::setGuiObjectEnabled(bool _enabled, int _counter) {
	m_disabledCounter += (_enabled ? (-_counter) : _counter);
	this->evaluateEnabledState();
}

void ot::ManagedGuiObject::resetGuiObjectDisabledCounter(void) {
	m_disabledCounter = 0;
	this->evaluateEnabledState();
}

void ot::ManagedGuiObject::lockGuiObject(const LockTypeFlags& _flags, int _lockCount) {
	for (LockTypeFlag flag : ot::getAllLockTypeFlags()) {
		if (_flags.flagIsSet(flag)) this->lockGuiObjectFlag(flag, _lockCount);
	}
	this->evaluateEnabledState();
}

void ot::ManagedGuiObject::unlockGuiObject(const LockTypeFlags& _flags, int _unlockCount) {
	for (LockTypeFlag flag : ot::getAllLockTypeFlags()) {
		if (_flags.flagIsSet(flag)) this->unlockGuiObjectFlag(flag, _unlockCount);
	}
	this->evaluateEnabledState();
}

void ot::ManagedGuiObject::resetGuiObjectLockCounter(void) {
	for (auto& it : m_lockCounter) {
		it.second = 0;
	}
	this->evaluateEnabledState();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private

void ot::ManagedGuiObject::lockGuiObjectFlag(LockTypeFlag _flag, int _lockCount) {
	int& ct = this->getLockCounter(_flag);
	ct = ct + _lockCount;
	this->evaluateEnabledState();
}

void ot::ManagedGuiObject::unlockGuiObjectFlag(LockTypeFlag _flag, int _unlockCount) {
	int& ct = this->getLockCounter(_flag);
	ct = ct - _unlockCount;
	this->evaluateEnabledState();
}

void ot::ManagedGuiObject::evaluateEnabledState(void) {
	OTAssert(m_disabledCounter >= 0, "Disabled counter out of range");

	bool enabled = m_disabledCounter > 0;
	bool unlocked = true;
	for (const auto& it : m_lockCounter) {
		OTAssert(it.second >= 0, "Lock counter out of range");

		if (it.second > 0) {
			unlocked = false;
			break;
		}
	}

	if ((enabled && unlocked) != m_isEnabled) {
		m_isEnabled = (enabled && unlocked);
		this->updateGuiObjectEnabledState(m_isEnabled);
	}
}

int& ot::ManagedGuiObject::getLockCounter(LockTypeFlag _flag) {
	auto it = m_lockCounter.find(_flag);
	if (it == m_lockCounter.end()) {
		m_lockCounter.insert_or_assign(_flag, 0);
		return this->getLockCounter(_flag);
	}
	else {
		return it->second;
	}
}
