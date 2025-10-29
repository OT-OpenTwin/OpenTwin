// @otlicense

//! @file IconManager.hpp
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/IconManager.h"

template <class T>
std::shared_ptr<T>& ot::IconManager::getOrCreate(const QString& _subPath, std::map<QString, std::shared_ptr<T>>& _dataMap, std::shared_ptr<T>& _default) {
	std::lock_guard<std::mutex> lock(this->m_mutex);
	
	// Find existing
	std::optional<std::shared_ptr<T>*> existing = this->getWhileLocked<T>(_subPath, _dataMap);
	if (existing.has_value()) {
		if (existing.value()) {
			return *existing.value();
		}
	}

	// Find new
	QString path = this->findFullPath(_subPath);
	if (path.isEmpty()) {
		OT_LOG_E("Icon \"" + _subPath.toStdString() + "\" not found");
		return _default;
	}

	// Create and store new
	std::shared_ptr<T> newShared(new T(path));
	auto result = _dataMap.insert_or_assign(_subPath, newShared);
	return result.first->second;
}

template <class T>
std::optional<std::shared_ptr<T>*> ot::IconManager::get(const QString& _subPath, std::map<QString, std::shared_ptr<T>>& _dataMap) {
	std::lock_guard<std::mutex> lock(this->m_mutex);
	return this->getWhileLocked<T>(_subPath, _dataMap);
}

template <class T>
std::optional<std::shared_ptr<T>*> ot::IconManager::getWhileLocked(const QString& _subPath, std::map<QString, std::shared_ptr<T>>& _dataMap) {
	// Find existing
	const auto& it = _dataMap.find(_subPath);
	if (it != _dataMap.end()) {
		return &it->second;
	}
	else {
		return std::nullopt;
	}
}