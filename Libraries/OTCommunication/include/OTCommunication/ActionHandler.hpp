// @otlicense
// File: ActionHandler.hpp
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

#pragma once

// OpenTwin header
#include "OTCommunication/ActionHandler.h"
#include "OTCommunication/ActionDispatcher.h"
#include "OTCommunication/ActionHandleConnector.h"

namespace ot {

	namespace intern {
		template<typename Function> struct ActionHandlerFunctionWrapper;

		// void(void)
		template<>
		struct ActionHandlerFunctionWrapper<void(*)()> {
			static ActionHandler::ActionHandlerMethodType wrap(void(*_func)()) {
				return [_func](JsonDocument&) -> std::string {
					_func();
					return ot::ReturnMessage::toJson(ot::ReturnMessage::Ok);
					};
			}
		};

		// void(T::*)(void)
		template<typename T>
		struct ActionHandlerFunctionWrapper<void(T::*)()> {
			static ActionHandler::ActionHandlerMethodType wrap(void(T::* _method)(), T* _instance) {
				return [_instance, _method](JsonDocument&) -> std::string {
					(_instance->*_method)();
					return ot::ReturnMessage::toJson(ot::ReturnMessage::Ok);
					};
			}
		};

		// void(JsonDocument&)
		template<>
		struct ActionHandlerFunctionWrapper<void(*)(JsonDocument&)> {
			static ActionHandler::ActionHandlerMethodType wrap(void(*_func)(JsonDocument&)) {
				return [_func](JsonDocument& doc) -> std::string {
					_func(doc);
					return ot::ReturnMessage::toJson(ot::ReturnMessage::Ok);
					};
			}
		};

		// void(T::*)(JsonDocument&)
		template<typename T>
		struct ActionHandlerFunctionWrapper<void(T::*)(JsonDocument&)> {
			static ActionHandler::ActionHandlerMethodType wrap(void(T::* _method)(JsonDocument&), T* _instance) {
				return [_instance, _method](JsonDocument& doc) -> std::string {
					(_instance->*_method)(doc);
					return ot::ReturnMessage::toJson(ot::ReturnMessage::Ok);
					};
			}
		};

		// std::string(void)
		template<>
		struct ActionHandlerFunctionWrapper<std::string(*)()> {
			static ActionHandler::ActionHandlerMethodType wrap(std::string(*_func)()) {
				return [_func](JsonDocument&) -> std::string {
					return _func();
					};
			}
		};

		// std::string(T::*)(void) 
		template<typename T>
		struct ActionHandlerFunctionWrapper<std::string(T::*)()> {
			static ActionHandler::ActionHandlerMethodType wrap(std::string(T::* _method)(), T* _instance) {
				return [_instance, _method](JsonDocument&) -> std::string {
					return (_instance->*_method)();
					};
			}
		};

		// std::string(JsonDocument&)
		template<>
		struct ActionHandlerFunctionWrapper<std::string(*)(JsonDocument&)> {
			static ActionHandler::ActionHandlerMethodType wrap(std::string(*_func)(JsonDocument&)) {
				return [_func](JsonDocument& doc) -> std::string {
					return _func(doc);
					};
			}
		};

		// std::string(T::*)(JsonDocument&)
		template<typename T>
		struct ActionHandlerFunctionWrapper<std::string(T::*)(JsonDocument&)> {
			static ActionHandler::ActionHandlerMethodType wrap(std::string(T::* _method)(JsonDocument&), T* _instance) {
				return [_instance, _method](JsonDocument& doc) -> std::string {
					return (_instance->*_method)(doc);
					};
			}
		};

		// ot::ReturnMessage(void)
		template<>
		struct ActionHandlerFunctionWrapper<ot::ReturnMessage(*)()> {
			static ActionHandler::ActionHandlerMethodType wrap(ot::ReturnMessage(*_func)()) {
				return [_func](JsonDocument&) -> std::string {
					return _func().toJson();
					};
			}
		};

		// ot::ReturnMessage(JsonDocument&)
		template<>
		struct ActionHandlerFunctionWrapper<ot::ReturnMessage(*)(JsonDocument&)> {
			static ActionHandler::ActionHandlerMethodType wrap(ot::ReturnMessage(*_func)(JsonDocument&)) {
				return [_func](JsonDocument& doc) -> std::string {
					return _func(doc).toJson();
					};
			}
		};

		// ot::ReturnMessage(T::*)(void)
		template<typename T>
		struct ActionHandlerFunctionWrapper<ot::ReturnMessage(T::*)()> {
			static ActionHandler::ActionHandlerMethodType wrap(ot::ReturnMessage(T::* _method)(), T* _instance) {
				return [_instance, _method](JsonDocument&) -> std::string {
					return (_instance->*_method)().toJson();
					};
			}
		};

		// ot::ReturnMessage(T::*)(JsonDocument&)
		template<typename T>
		struct ActionHandlerFunctionWrapper<ot::ReturnMessage(T::*)(JsonDocument&)> {
			static ActionHandler::ActionHandlerMethodType wrap(ot::ReturnMessage(T::* _method)(JsonDocument&), T* _instance) {
				return [_instance, _method](JsonDocument& doc) -> std::string {
					return (_instance->*_method)(doc).toJson();
					};
			}
		};

		// Specialization for std::function (to handle the case when connect is called with existing ActionHandlerMethodType)
		template<>
		struct ActionHandlerFunctionWrapper<ActionHandler::ActionHandlerMethodType> {
			static ActionHandler::ActionHandlerMethodType wrap(const ActionHandler::ActionHandlerMethodType& _func) {
				return _func;
			}
		};

		// Generic callable / lambda support
		template<typename Callable>
		struct ActionHandlerFunctionWrapper {
			static ActionHandler::ActionHandlerMethodType wrap(Callable&& func) {
				return [f = std::forward<Callable>(func)](JsonDocument& _doc) -> std::string {
					if constexpr (std::is_invocable_r_v<void, Callable>) {
						f();
						return ot::ReturnMessage::toJson(ot::ReturnMessage::Ok);
					}
					else if constexpr (std::is_invocable_r_v<void, Callable, JsonDocument&>) {
						f(_doc);
						return ot::ReturnMessage::toJson(ot::ReturnMessage::Ok);
					}
					else if constexpr (std::is_invocable_r_v<std::string, Callable>) {
						return f();
					}
					else if constexpr (std::is_invocable_r_v<std::string, Callable, JsonDocument&>) {
						return f(_doc);
					}
					else if constexpr (std::is_invocable_r_v<ot::ReturnMessage, Callable>) {
						return f().toJson();
					}
					else if constexpr (std::is_invocable_r_v<ot::ReturnMessage, Callable, JsonDocument&>) {
						return f(_doc).toJson();
					}
					else {
						static_assert(sizeof(Callable) == 0,
							"Unsupported lambda/functor signature.\n"
							"Supported forms:\n"
							"  void(), void(JsonDocument&)\n"
							"  std::string(), std::string(JsonDocument&)\n"
							"  ot::ReturnMessage(), ot::ReturnMessage(JsonDocument&)\n");
					}
				};
			}
		};
	}

}

inline ot::ActionHandler::ActionHandler(ot::ActionDispatcherBase* _dispatcher) 
	: m_dispatcher(nullptr)
{
	m_dispatcher = (_dispatcher ? _dispatcher : &ot::ActionDispatcher::instance());
}

inline ot::ActionHandler::ActionHandler(ActionHandler&& _other) noexcept : m_dispatcher(nullptr) {
	this->operator=(std::move(_other));
}

inline ot::ActionHandler::~ActionHandler() {
	while (!m_handlers.empty()) {
		ot::ActionHandleConnector* con = m_handlers.front();
		m_handlers.pop_front();
		if (m_dispatcher) {
			m_dispatcher->remove(con);
		}
		delete con;
	}
}

inline ot::ActionHandler& ot::ActionHandler::operator=(ActionHandler&& _other) noexcept {
	if (this != &_other) {
		m_handlers = std::move(_other.m_handlers);
		m_dispatcher = _other.m_dispatcher;

		_other.m_dispatcher = nullptr;
		_other.m_handlers.clear();
	}
	return *this;
}

inline bool ot::ActionHandler::connectActionImpl(const std::list<std::string>& _actionNames, const ActionHandlerMethodType& _method, const MessageTypeFlags& _messageFlags, const ActionDispatcherBase::InsertFlags& _insertFlags) {
	ot::ActionHandleConnector* con = new ot::ActionHandleConnector(_actionNames, _method, _messageFlags);

	if (m_dispatcher->add(con, _insertFlags)) {
		m_handlers.push_back(con);
		return true;
	}
	else {
		delete con;
		return false;
	}
}

// Primary template implementation

template<typename Function>
inline bool ot::ActionHandler::connectAction(const std::string& _actionName, Function&& _func, const MessageTypeFlags& _messageFlags, const ActionDispatcherBase::InsertFlags& _insertFlags) {
	auto wrapper = ot::intern::ActionHandlerFunctionWrapper<std::decay_t<Function>>::wrap(std::forward<Function>(_func));
	return this->connectActionImpl(std::list<std::string>({ _actionName }), wrapper, _messageFlags, _insertFlags);
}

template<typename Function>
inline bool ot::ActionHandler::connectAction(const std::initializer_list<std::string>& _actionNames, Function&& _func, const MessageTypeFlags& _messageFlags, const ActionDispatcherBase::InsertFlags& _insertFlags) {
	auto wrapper = ot::intern::ActionHandlerFunctionWrapper<std::decay_t<Function>>::wrap(std::forward<Function>(_func));
	return this->connectActionImpl(std::list<std::string>(_actionNames), wrapper, _messageFlags, _insertFlags);
}

template<typename Function>
inline bool ot::ActionHandler::connectAction(const std::list<std::string>& _actionNames, Function&& _func, const MessageTypeFlags& _messageFlags, const ActionDispatcherBase::InsertFlags& _insertFlags) {
	auto wrapper = ot::intern::ActionHandlerFunctionWrapper<std::decay_t<Function>>::wrap(std::forward<Function>(_func));
	return this->connectActionImpl(_actionNames, wrapper, _messageFlags, _insertFlags);
}

// Member function version

template<typename T, typename Method>
inline bool ot::ActionHandler::connectAction(const std::string& _actionName, T* _instance, Method&& _method, const MessageTypeFlags& _messageFlags, const ActionDispatcherBase::InsertFlags& _insertFlags) {
	auto wrapper = ot::intern::ActionHandlerFunctionWrapper<std::decay_t<Method>>::wrap(std::forward<Method>(_method), _instance);
	return this->connectActionImpl(std::list<std::string>({ _actionName }), wrapper, _messageFlags, _insertFlags);
}

template<typename T, typename Method>
inline bool ot::ActionHandler::connectAction(const std::initializer_list<std::string>& _actionNames, T* _instance, Method&& _method, const MessageTypeFlags& _messageFlags, const ActionDispatcherBase::InsertFlags& _insertFlags) {
	auto wrapper = ot::intern::ActionHandlerFunctionWrapper<std::decay_t<Method>>::wrap(std::forward<Method>(_method), _instance);
	return this->connectActionImpl(std::list<std::string>(_actionNames), wrapper, _messageFlags, _insertFlags);
}

template<typename T, typename Method>
inline bool ot::ActionHandler::connectAction(const std::list<std::string>& _actionNames, T* _instance, Method&& _method, const MessageTypeFlags& _messageFlags, const ActionDispatcherBase::InsertFlags& _insertFlags) {
	auto wrapper = ot::intern::ActionHandlerFunctionWrapper<std::decay_t<Method>>::wrap(std::forward<Method>(_method), _instance);
	return this->connectActionImpl(_actionNames, wrapper, _messageFlags, _insertFlags);
}
