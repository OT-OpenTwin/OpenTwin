// @otlicense

//! @file WidgetManager.cpp
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTWidgets/Widget.h"
#include "OTWidgets/WidgetManager.h"

ot::WidgetManager::WidgetManager() {

}

ot::WidgetManager::~WidgetManager() {
	// Lock reset should be 
}

void ot::WidgetManager::registerWidget(Widget* _widget) {
	OTAssertNullptr(_widget);

	// Check for duplicates
	if (this->findWidget(_widget->getOTObjectName()) != nullptr) {
		OT_LOG_E("Widget \"" + _widget->getOTObjectName() + "\" already registered");
		return;
	}

	// Store data
	m_widgets.insert_or_assign(_widget->getOTObjectName(), _widget);

	OwnerData& ownerData = this->findOrCreateOwnerData(_widget->getWidgetOwner());
	ownerData.widgets.push_back(_widget);

	// Update lock
}

void ot::WidgetManager::deregisterWidget(Widget* _widget) {
	this->clearWidgetInfo(_widget);
}

void ot::WidgetManager::ownerDisconnected(const BasicServiceInformation& _owner) {
	std::list<Widget*> ownedWidgets;
	{
		OwnerData& ownerData = this->findOrCreateOwnerData(_owner);

		// Reset locks
		for (auto& it : ownerData.lockData) {
			OTAssert(it.second >= 0, "Invalid lock count stored");
			if (it.second > 0) {
				for (auto& widgetInfo : m_widgets) {
					widgetInfo.second->unlockWidget(it.first, it.second);
				}
			}
		}

		// Copy owned objects
		ownedWidgets = ownerData.widgets;
	}

	// Destroy owned objects
	for (Widget* widget : ownedWidgets) {
		this->clearWidgetInfo(widget);
	}
}

ot::Widget* ot::WidgetManager::findWidget(const std::string& _name) const {
	auto it = m_widgets.find(_name);
	if (it == m_widgets.end()) {
		return nullptr;
	}
	else {
		return it->second;
	}
}

void ot::WidgetManager::lock(const BasicServiceInformation& _owner, const ot::LockTypes& _lockFlags) {
	OwnerData& ownerData = this->findOrCreateOwnerData(_owner);
	std::list<LockType> flags = getAllSetFlags(_lockFlags);
	for (LockType flag : flags) {
		int& count = this->getLockCount(ownerData, flag);
		count++;

		for (auto& widgetInfo : m_widgets) {
			widgetInfo.second->lockWidget(flag, 1);
		}
	}
}

void ot::WidgetManager::unlock(const BasicServiceInformation& _owner, const ot::LockTypes& _lockFlags) {
	OwnerData& ownerData = this->findOrCreateOwnerData(_owner);
	std::list<LockType> flags = getAllSetFlags(_lockFlags);
	for (LockType flag : flags) {
		int& count = this->getLockCount(ownerData, flag);
		if (count == 0) {
			OT_LOG_E("Unlock called without a previous lock");
			continue;
		}
		count++;

		for (auto& widgetInfo : m_widgets) {
			widgetInfo.second->unlockWidget(flag, 1);
		}
	}
}

void ot::WidgetManager::objectWasDestroyed(OTObject* _object) {
	Widget* actualObject = dynamic_cast<Widget*>(_object);
	if (!actualObject) {
		OT_LOG_E("Object is not a widget");
	}
	else {
		this->clearWidgetInfo(actualObject);
	}
}

void ot::WidgetManager::clearWidgetInfo(Widget* _widget) {
	// Remove from widgets
	auto it = m_widgets.find(_widget->getOTObjectName());
	if (it != m_widgets.end()) {
		m_widgets.erase(it);
	}

	// Remove from owner list
	for (auto& owner : m_ownerData) {
		auto it = std::find(owner.second.widgets.begin(), owner.second.widgets.end(), _widget);
		if (it != owner.second.widgets.end()) {
			owner.second.widgets.erase(it);
		}
	}
}

ot::WidgetManager::OwnerData& ot::WidgetManager::findOrCreateOwnerData(const BasicServiceInformation& _owner) {
	auto it = m_ownerData.find(_owner);
	if (it == m_ownerData.end()) {
		OwnerData newData;
		return m_ownerData.insert_or_assign(_owner, std::move(newData)).first->second;
	}
	else {
		return it->second;
	}
}

int& ot::WidgetManager::getLockCount(OwnerData& _ownerData, LockType _lockFlag) {
	auto it = _ownerData.lockData.find(_lockFlag);
	if (it == _ownerData.lockData.end()) {
		return _ownerData.lockData.insert_or_assign(_lockFlag, 0).first->second;
	}
	else {
		return it->second;
	}
}
