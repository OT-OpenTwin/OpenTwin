//! @file ActionDispatcherBase.h
//! @author Alexander Kuester (alexk95)
//! @date January 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTSystem/Flags.h"
#include "OTCore/JSON.h"
#include "OTCore/OTClassHelper.h"
#include "OTCore/ReturnMessage.h"
#include "OTCommunication/CommunicationTypes.h"
#include "OTCommunication/CommunicationAPIExport.h"

// std header
#include <map>
#include <mutex>
#include <optional>
#include <functional>

#pragma warning(disable:4251)

namespace ot {

	class ActionHandleConnector;

	//! @brief Main action dispatcher base class.
	//! The ActionDispatcherBase is used to forward actions to the corresponding [\ref ActionHanderConnector "handler"].
	//! A mutex ensures that only a single message can be handled at a time.
	class OT_COMMUNICATION_API_EXPORT ActionDispatcherBase {
		OT_DECL_NOCOPY(ActionDispatcherBase)
		OT_DECL_NOMOVE(ActionDispatcherBase)
	public:
		enum InsertFlag {
			NoFlags        = 0 << 0, //! @brief No flags set.
			ExpectMultiple = 1 << 0  //! @brief If set, the dispatcher will expect multiple connectors for the same action name.
		};
		typedef ot::Flags<InsertFlag> InsertFlags;

		typedef std::function<std::string(JsonDocument&)> DispatchMethodType;

		ActionDispatcherBase();
		virtual ~ActionDispatcherBase();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Connector management

		//! @brief Add the provided [\ref ActionHandleConnector "handler"] to this ActionDispatcherBase.
		//! The handler will be used for all specified actions.
		//! @param _handler New handler to add for all actions set in the [\ref ActionHandleConnector "handler"].
		//! The caller keeps ownership of the handler.
		//! @param _insertFlags Flags to control the insert behavior.
		std::optional<std::shared_ptr<ActionHandleConnector>> add(ActionHandleConnector* _handler, const InsertFlags& _insertFlags = InsertFlag::NoFlags);

		//! @brief Creates a new [\ref ActionHandleConnector "handler"] and adds it to this ActionDispatcherBase.
		//! @tparam Function The type of the function to be called when a message is received.
		//! @param _actionName Name of the action to bind the handler to.
		//! @param _messageFlags The message type flags the connector will accept.
		//! @param _func The function to be called when a message is received.
		//! @param _insertFlags Flags to control the insert behavior.
		//! @return The created handler.
		template<typename Function>
		std::shared_ptr<ActionHandleConnector> connect(const std::string& _actionName, const MessageTypeFlags& _messageFlags, Function&& _func, const InsertFlags& _insertFlags = InsertFlag::NoFlags);

		//! @brief Creates a new [\ref ActionHandleConnector "handler"] and adds it to this ActionDispatcherBase.
		//! @tparam Function The type of the function to be called when a message is received.
		//! @param _actionName Names of the actions to bind the handler to.
		//! @param _messageFlags The message type flags the connector will accept.
		//! @param _func The function to be called when a message is received.
		//! @param _insertFlags Flags to control the insert behavior.
		//! @return The created handler.
		template<typename Function>
		std::shared_ptr<ActionHandleConnector> connect(const std::initializer_list<std::string>& _actionNames, const MessageTypeFlags& _messageFlags, Function&& _func, const InsertFlags& _insertFlags = InsertFlag::NoFlags);

		//! @brief Creates a new [\ref ActionHandleConnector "handler"] and adds it to this ActionDispatcherBase.
		//! @tparam Function The type of the function to be called when a message is received.
		//! @param _actionName Names of the actions to bind the handler to.
		//! @param _messageFlags The message type flags the connector will accept.
		//! @param _func The function to be called when a message is received.
		//! @param _insertFlags Flags to control the insert behavior.
		//! @return The created handler.
		template<typename Function>
		std::shared_ptr<ActionHandleConnector> connect(const std::list<std::string>& _actionNames, const MessageTypeFlags& _messageFlags, Function&& _func, const InsertFlags& _insertFlags = InsertFlag::NoFlags);

		//! @brief Creates a new [\ref ActionHandleConnector "handler"] and adds it to this ActionDispatcherBase.
		//! @tparam T Class type of the instance the method belongs to.
		//! @tparam Method The type of the method to be called when a message is received.
		//! @param _actionName Name of the action to bind the handler to.
		//! @param _messageFlags The message type flags the connector will accept.
		//! @param _instance Instance the method belongs to.
		//! @param _method The method to be called when a message is received.
		//! @param _insertFlags Flags to control the insert behavior.
		//! @return The created handler.
		template<typename T, typename Method>
		std::shared_ptr<ActionHandleConnector> connect(const std::string& _actionName, const MessageTypeFlags& _messageFlags, T* _instance, Method&& _method, const InsertFlags& _insertFlags = InsertFlag::NoFlags);

		//! @brief Creates a new [\ref ActionHandleConnector "handler"] and adds it to this ActionDispatcherBase.
		//! @tparam T Class type of the instance the method belongs to.
		//! @tparam Method The type of the method to be called when a message is received.
		//! @param _actionNames Names of the actions to bind the handler to.
		//! @param _messageFlags The message type flags the connector will accept.
		//! @param _instance Instance the method belongs to.
		//! @param _method The method to be called when a message is received.
		//! @param _insertFlags Flags to control the insert behavior.
		//! @return The created handler.
		template<typename T, typename Method>
		std::shared_ptr<ActionHandleConnector> connect(const std::initializer_list<std::string>& _actionNames, const MessageTypeFlags& _messageFlags, T* _instance, Method&& _method, const InsertFlags& _insertFlags = InsertFlag::NoFlags);

		//! @brief Creates a new [\ref ActionHandleConnector "handler"] and adds it to this ActionDispatcherBase.
		//! @tparam T Class type of the instance the method belongs to.
		//! @tparam Method The type of the method to be called when a message is received.
		//! @param _actionNames Names of the actions to bind the handler to.
		//! @param _messageFlags The message type flags the connector will accept.
		//! @param _instance Instance the method belongs to.
		//! @param _method The method to be called when a message is received.
		//! @param _insertFlags Flags to control the insert behavior.
		//! @return The created handler.
		template<typename T, typename Method>
		std::shared_ptr<ActionHandleConnector> connect(const std::list<std::string>& _actionNames, const MessageTypeFlags& _messageFlags, T* _instance, Method&& _method, const InsertFlags& _insertFlags = InsertFlag::NoFlags);

		//! @brief Removes the [\ref ActionHandleConnector "handler"] from the list.
		//! This must be called when the [\ref ActionHandleConnector "handler"] is destroyed.
		//! @param _handler Handler to remove from the lists.
		void remove(ActionHandleConnector* _handler);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Dispatching

		//! @brief This function may be called from the main external API that is receiving the perform/ queue messages
		//! This function will forward the call to the dispatch function and will create a C-String copy of the result so
		//! the string can be directy returned out of the dll/application
		char* dispatchWrapper(const char* _json, const char* _senderUrl, MessageType _messageType);

		//! @brief Will get and forward the action to all registered handlers
		std::string dispatch(const std::string& _json, MessageType _messageType);

		//! @brief Will get and forward the action to all registered handlers
		//! @param _document The document to dispatch. Note that the document may be invalid if a handler was found.
		std::string dispatch(JsonDocument& _document, MessageType _messageType);

		//! @brief Will forward the action to all registered handlers
		//! Will return the result of the last handler
		//! Uses a mutex to ensure that only one request is handled at a time
		//! @param _action The action to dispatch
		//! @param _document The document to dispatch. Note that the document may be invalid if a handler was found.
		//! @param _messageType MessageTypeFlags describing the endpoint this message was received on.
		std::string dispatch(const std::string& _action, JsonDocument& _document, MessageType _messageType);

		//! @brief Will forward the action to all registered handlers
		//! Will return the result of the last handler
		//! Uses a mutex to ensure that only one request is handled at a time
		//! @param _action The action to dispatch
		//! @param _document The document containing the action parameter
		//! @param _handlerFound Will be set to true if at least one handler was found to dispatch this action to
		std::string dispatch(const std::string& _action, JsonDocument& _document, bool& _handlerFound, MessageType _messageType);

		//! @brief Calls the dispatch implementation assuming the mutex is already locked.
		//! @note Unsafe, only call from a handler when the mutex is already locked!
		//! @param _action The action to dispatch.
		//! @param _document The document containing the action parameter.
		//! @param _handlerFound Will be set to true if at least one handler was found to dispatch this action to.
		std::string dispatchLocked(const std::string& _action, JsonDocument& _document, bool& _handlerFound, MessageType _messageType) { return this->dispatchImpl(_action, _document, _handlerFound, _messageType); };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Protected: Dispatch implementation

	protected:

		//! @brief Performs the dispatch.
		//! All previous dispatch methods call this method.
		//! Override if custom dispatch handling is required.
		//! @note Unsafe, only call from a handler when the mutex is already locked!
		//! @param _action The action to dispatch.
		//! @param _document The document containing the action parameter.
		//! @param _handlerFound Will be set to true if at least one handler was found to dispatch this action to.
		virtual std::string dispatchImpl(const std::string& _action, JsonDocument& _document, bool& _handlerFound, MessageType _messageType);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private: Connect implementation

	private:
		//! @brief Creates a new [\ref ActionHandleConnector "handler"] and adds it to this ActionDispatcherBase.
		//! @param _actionName Name of the action to bind the handler to.
		//! @param _messageFlags The message type flags the connector will accept.
		//! @param _method The method to be called when a message is received.
		//! @param _insertFlags Flags to control the insert behavior.
		//! @return The created handler. The dispatcher keeps ownership of the handler. Call ActionDispatcherBase::remove() to take ownership.
		std::shared_ptr<ActionHandleConnector> connectImpl(const std::list<std::string>& _actionNames, const MessageTypeFlags& _messageFlags, const DispatchMethodType& _method, const InsertFlags& _insertFlags = InsertFlag::NoFlags);

		std::mutex m_mutex;
		std::map<std::string, std::list<std::shared_ptr<ActionHandleConnector>>> m_data;
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::ActionDispatcherBase::InsertFlag)

// ###########################################################################################################################################################################################################################################################################################################################

// Connect overloads

namespace ot {
	namespace intern {
		template<typename Function> struct HandlerFunctionWrapper;

		//! @brief Helper function to call member functions with different signatures.
		template<typename T, typename Method>
		auto callHandlerFunction(T* _instance, Method _method, JsonDocument& _doc) -> std::string {
			if constexpr (std::is_invocable_v<Method, T*, JsonDocument&>) {
				// For methods taking JsonDocument&
				if constexpr (std::is_same_v<std::invoke_result_t<Method, T*, JsonDocument&>, void>) {
					(_instance->*_method)(_doc);
					return ot::ReturnMessage::toJson(ot::ReturnMessage::Ok);
				}
				else if constexpr (std::is_same_v<std::invoke_result_t<Method, T*, JsonDocument&>, ot::ReturnMessage>) {
					return (_instance->*_method)(_doc).toJson();
				}
				else {
					return (_instance->*_method)(_doc);
				}
			}
			else {
				// For methods taking no arguments
				if constexpr (std::is_same_v<std::invoke_result_t<Method, T*>, void>) {
					(_instance->*_method)();
					return ot::ReturnMessage::toJson(ot::ReturnMessage::Ok);
				}
				else if constexpr (std::is_same_v<std::invoke_result_t<Method, T*>, ot::ReturnMessage>) {
					return (_instance->*_method)().toJson();
				}
				else {
					return (_instance->*_method)();
				}
			}
		}

		// Primary template that will fail for unsupported types
		template<typename Function>
		struct HandlerFunctionWrapper {
			// No wrap method - this will cause compile error for unsupported types
			static_assert(sizeof(Function) == 0, "Unsupported function type for HandlerFunctionWrapper.\n"
				"Supported types are:\n"
				" - void(void)\n"
				" - void(T::*)(void)\n"
				" - void(JsonDocument&)\n"
				" - void(T::*)(JsonDocument&)\n"
				"\n"
				" - std::string(void)\n"
				" - std::string(T::*)(void)\n"
				" - std::string(JsonDocument&)\n"
				" - std::string(T::*)(JsonDocument&)\n"
				"\n"
				" - ot::ReturnMessage(void)\n"
				" - ot::ReturnMessage(T::*)(void)\n"
				" - ot::ReturnMessage(JsonDocument&)\n"
				" - ot::ReturnMessage(T::*)(JsonDocument&)\n"
				);
		};

		// void(void)
		template<>
		struct HandlerFunctionWrapper<void(*)()> {
			static ActionDispatcherBase::DispatchMethodType wrap(void(*_func)()) {
				return [_func](JsonDocument&) -> std::string {
					_func();
					return ot::ReturnMessage::toJson(ot::ReturnMessage::Ok);
					};
			}
		};

		// void(T::*)(void)
		template<typename T>
		struct HandlerFunctionWrapper<void(T::*)()> {
			static ActionDispatcherBase::DispatchMethodType wrap(void(T::* method)(), T* instance) {
				return [instance, method](JsonDocument&) -> std::string {
					(instance->*method)();
					return ot::ReturnMessage::toJson(ot::ReturnMessage::Ok);
					};
			}
		};

		// void(JsonDocument&)
		template<>
		struct HandlerFunctionWrapper<void(*)(JsonDocument&)> {
			static ActionDispatcherBase::DispatchMethodType wrap(void(*func)(JsonDocument&)) {
				return [func](JsonDocument& doc) -> std::string {
					func(doc);
					return ot::ReturnMessage::toJson(ot::ReturnMessage::Ok);
					};
			}
		};

		// void(T::*)(JsonDocument&)
		template<typename T>
		struct HandlerFunctionWrapper<void(T::*)(JsonDocument&)> {
			static ActionDispatcherBase::DispatchMethodType wrap(void(T::* method)(JsonDocument&), T* instance) {
				return [instance, method](JsonDocument& doc) -> std::string {
					(instance->*method)(doc);
					return ot::ReturnMessage::toJson(ot::ReturnMessage::Ok);
					};
			}
		};

		// std::string(void)
		template<>
		struct HandlerFunctionWrapper<std::string(*)()> {
			static ActionDispatcherBase::DispatchMethodType wrap(std::string(*func)()) {
				return [func](JsonDocument&) -> std::string {
					return func();
					};
			}
		};

		// std::string(T::*)(void) 
		template<typename T>
		struct HandlerFunctionWrapper<std::string(T::*)()> {
			static ActionDispatcherBase::DispatchMethodType wrap(std::string(T::* method)(), T* instance) {
				return [instance, method](JsonDocument&) -> std::string {
					return (instance->*method)();
					};
			}
		};

		// std::string(JsonDocument&)
		template<>
		struct HandlerFunctionWrapper<std::string(*)(JsonDocument&)> {
			static ActionDispatcherBase::DispatchMethodType wrap(std::string(*func)(JsonDocument&)) {
				return [func](JsonDocument& doc) -> std::string {
					return func(doc);
					};
			}
		};

		// std::string(T::*)(JsonDocument&)
		template<typename T>
		struct HandlerFunctionWrapper<std::string(T::*)(JsonDocument&)> {
			static ActionDispatcherBase::DispatchMethodType wrap(std::string(T::* method)(JsonDocument&), T* instance) {
				return [instance, method](JsonDocument& doc) -> std::string {
					return (instance->*method)(doc);
					};
			}
		};

		// ot::ReturnMessage(void)
		template<>
		struct HandlerFunctionWrapper<ot::ReturnMessage(*)()> {
			static ActionDispatcherBase::DispatchMethodType wrap(ot::ReturnMessage(*func)()) {
				return [func](JsonDocument&) -> std::string {
					return func().toJson();
					};
			}
		};

		// ot::ReturnMessage(JsonDocument&)
		template<>
		struct HandlerFunctionWrapper<ot::ReturnMessage(*)(JsonDocument&)> {
			static ActionDispatcherBase::DispatchMethodType wrap(ot::ReturnMessage(*func)(JsonDocument&)) {
				return [func](JsonDocument& doc) -> std::string {
					return func(doc).toJson();
					};
			}
		};

		// ot::ReturnMessage(T::*)(void)
		template<typename T>
		struct HandlerFunctionWrapper<ot::ReturnMessage(T::*)()> {
			static ActionDispatcherBase::DispatchMethodType wrap(ot::ReturnMessage(T::* method)(), T* instance) {
				return [instance, method](JsonDocument&) -> std::string {
					return (instance->*method)().toJson();
					};
			}
		};

		// ot::ReturnMessage(T::*)(JsonDocument&)
		template<typename T>
		struct HandlerFunctionWrapper<ot::ReturnMessage(T::*)(JsonDocument&)> {
			static ActionDispatcherBase::DispatchMethodType wrap(ot::ReturnMessage(T::* method)(JsonDocument&), T* instance) {
				return [instance, method](JsonDocument& doc) -> std::string {
					return (instance->*method)(doc).toJson();
					};
			}
		};

		// Specialization for std::function (to handle the case when connect is called with existing DispatchMethodType)
		template<>
		struct HandlerFunctionWrapper<ActionDispatcherBase::DispatchMethodType> {
			static ActionDispatcherBase::DispatchMethodType wrap(const ActionDispatcherBase::DispatchMethodType& func) {
				return func;
			}
		};

	}
}

// Primary template implementation
template<typename Function>
inline std::shared_ptr<ot::ActionHandleConnector> ot::ActionDispatcherBase::connect(const std::string& _actionName, const MessageTypeFlags& _messageFlags, Function&& _func, const InsertFlags& _insertFlags) {
	auto wrapper = ot::intern::HandlerFunctionWrapper<std::decay_t<Function>>::wrap(std::forward<Function>(_func));
	return this->connectImpl(std::list<std::string>({ _actionName }), _messageFlags, wrapper, _insertFlags);
}
template<typename Function>
inline std::shared_ptr<ot::ActionHandleConnector> ot::ActionDispatcherBase::connect(const std::initializer_list<std::string>& _actionNames, const MessageTypeFlags& _messageFlags, Function&& _func, const InsertFlags& _insertFlags) {
	auto wrapper = ot::intern::HandlerFunctionWrapper<std::decay_t<Function>>::wrap(std::forward<Function>(_func));
	return this->connectImpl(std::list<std::string>(_actionNames), _messageFlags, wrapper, _insertFlags);
}
template<typename Function>
inline std::shared_ptr<ot::ActionHandleConnector> ot::ActionDispatcherBase::connect(const std::list<std::string>& _actionNames, const MessageTypeFlags& _messageFlags, Function&& _func, const InsertFlags& _insertFlags) {
	auto wrapper = ot::intern::HandlerFunctionWrapper<std::decay_t<Function>>::wrap(std::forward<Function>(_func));
	return this->connectImpl(_actionNames, _messageFlags, wrapper, _insertFlags);
}

// Member function version
template<typename T, typename Method>
inline std::shared_ptr<ot::ActionHandleConnector> ot::ActionDispatcherBase::connect(const std::string& _actionName, const MessageTypeFlags& _messageFlags, T* _instance, Method&& _method, const InsertFlags& _insertFlags) {
	auto wrapper = ot::intern::HandlerFunctionWrapper<std::decay_t<Method>>::wrap(std::forward<Method>(_method), _instance);
	return this->connectImpl(std::list<std::string>({ _actionName }), _messageFlags, wrapper, _insertFlags);
}
template<typename T, typename Method>
inline std::shared_ptr<ot::ActionHandleConnector> ot::ActionDispatcherBase::connect(const std::initializer_list<std::string>& _actionNames, const MessageTypeFlags& _messageFlags, T* _instance, Method&& _method, const InsertFlags& _insertFlags) {
	auto wrapper = ot::intern::HandlerFunctionWrapper<std::decay_t<Method>>::wrap(std::forward<Method>(_method), _instance);
	return this->connectImpl(std::list<std::string>(_actionNames), _messageFlags, wrapper, _insertFlags);
}
template<typename T, typename Method>
inline std::shared_ptr<ot::ActionHandleConnector> ot::ActionDispatcherBase::connect(const std::list<std::string>& _actionNames, const MessageTypeFlags& _messageFlags, T* _instance, Method&& _method, const InsertFlags& _insertFlags) {
	auto wrapper = ot::intern::HandlerFunctionWrapper<std::decay_t<Method>>::wrap(std::forward<Method>(_method), _instance);
	return this->connectImpl(_actionNames, _messageFlags, wrapper, _insertFlags);
}
