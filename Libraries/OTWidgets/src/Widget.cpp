// @otlicense

//! @file Widget.cpp
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTWidgets/Widget.h"

ot::Widget::Widget() :
	m_isEnabled(true), m_isUnlocked(true), m_disabledCounter(0) 
{}

ot::Widget::~Widget() {}

// ###########################################################################################################################################################################################################################################################################################################################

// State management

void ot::Widget::setWidgetEnabled(bool _enabled, int _counter) {
	m_disabledCounter += (_enabled ? (-_counter) : _counter);
	this->evaluateEnabledState();
}

void ot::Widget::resetWidgetDisabledCounter(void) {
	m_disabledCounter = 0;
	this->evaluateEnabledState();
}

void ot::Widget::lockWidget(const LockTypes& _flags, int _lockCount) {
	for (LockType flag : ot::getAllLockTypes()) {
		if (_flags.flagIsSet(flag)) this->lockWidgetFlag(flag, _lockCount);
	}
	this->evaluateEnabledState();
}

void ot::Widget::unlockWidget(const LockTypes& _flags, int _unlockCount) {
	for (LockType flag : ot::getAllLockTypes()) {
		if (_flags.flagIsSet(flag)) this->unlockWidgetFlag(flag, _unlockCount);
	}
	this->evaluateEnabledState();
}

void ot::Widget::resetWidgetLockCounter(void) {
	for (auto& it : m_lockCounter) {
		it.second = 0;
	}
	this->evaluateEnabledState();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private

void ot::Widget::lockWidgetFlag(LockType _flag, int _lockCount) {
	int& ct = this->getLockCounter(_flag);
	ct = ct + _lockCount;
	this->evaluateEnabledState();
}

void ot::Widget::unlockWidgetFlag(LockType _flag, int _unlockCount) {
	int& ct = this->getLockCounter(_flag);
	ct = ct - _unlockCount;
	this->evaluateEnabledState();
}

void ot::Widget::evaluateEnabledState(void) {
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

	if ((enabled != m_isEnabled) || (unlocked != m_isUnlocked)) {
		m_isEnabled = enabled;
		m_isUnlocked = unlocked;
		this->updateWidgetEnabledState(m_isEnabled, !m_isUnlocked);
	}
}

int& ot::Widget::getLockCounter(LockType _flag) {
	auto it = m_lockCounter.find(_flag);
	if (it == m_lockCounter.end()) {
		m_lockCounter.insert_or_assign(_flag, 0);
		return this->getLockCounter(_flag);
	}
	else {
		return it->second;
	}
}
