// @otlicense
// File: EntityCallbackBase.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

// OpenTwin header
#include "EntityCallbackBase.h"
#include "OTCore/LogDispatcher.h"

void ot::EntityCallbackBase::clearCallbacks(bool _supressChangedNotification) {
	m_callbackServices.clear();
	if (!_supressChangedNotification) {
		callbackDataChanged();
	}
}

void ot::EntityCallbackBase::registerCallback(Callback _callback, const std::string& _serviceName, bool _supressChangedNotification) {
	auto it = m_callbackServices.find(_callback);
	if (it != m_callbackServices.end()) {
		it->second.push_back(_serviceName);
	}
	else {
		m_callbackServices.emplace(_callback, std::list<std::string>{ _serviceName });
	}

	if (!_supressChangedNotification) {
		callbackDataChanged();
	}
}

void ot::EntityCallbackBase::registerCallbacks(const CallbackFlags& _callbacks, const std::string& _serviceName, bool _supressChangedNotification) {
	for (Callback cb = Callback::CallbackIteratorFirst; cb <= Callback::CallbackIteratorLast; cb = static_cast<Callback>(static_cast<uint64_t>(cb) * 2)) {
		if (_callbacks & cb) {
			auto it = m_callbackServices.find(cb);
			if (it != m_callbackServices.end()) {
				it->second.push_back(_serviceName);
			}
			else {
				m_callbackServices.emplace(cb, std::list<std::string>{ _serviceName });
			}
		}
	}

	for (const auto& pair : m_callbackServices) {
		std::string services;
		for (const auto& serviceName : pair.second) {
			if (!services.empty()) {
				services += ", ";
			}
			services += serviceName;
		}
	}

	if (!_supressChangedNotification) {
		callbackDataChanged();
	}
}

ot::EntityCallbackBase::CallbackFlags ot::EntityCallbackBase::getServiceCallbacks(const std::string& _serviceName) const {
	CallbackFlags callbacks(Callback::None);

	for (const auto& pair : m_callbackServices) {
		Callback cb = pair.first;
		const std::list<std::string>& services = pair.second;
		if (std::find(services.begin(), services.end(), _serviceName) != services.end()) {
			callbacks |= cb;
		}
	}

	return callbacks;
}

std::list<std::string> ot::EntityCallbackBase::getServicesForCallback(Callback _callback) const {
	auto it = m_callbackServices.find(_callback);
	if (it != m_callbackServices.end()) {
		return it->second;
	}
	else {
		return std::list<std::string>{};
	}
}

void ot::EntityCallbackBase::setCallbackData(const std::map<Callback, std::list<std::string>>& _callbackServices, bool _supressChangedNotification) {
	m_callbackServices = _callbackServices;
	if (!_supressChangedNotification) {
		callbackDataChanged();
	}
}

void ot::EntityCallbackBase::setCallbackData(std::map<Callback, std::list<std::string>>&& _callbackServices, bool _supressChangedNotification) {
	m_callbackServices = std::move(_callbackServices);
	if (!_supressChangedNotification) {
		callbackDataChanged();
	}
}

void ot::EntityCallbackBase::setCallbackData(const EntityCallbackBase& _other, bool _supressChangedNotification) {
	m_callbackServices = _other.m_callbackServices;
	if (!_supressChangedNotification) {
		callbackDataChanged();
	}
}
