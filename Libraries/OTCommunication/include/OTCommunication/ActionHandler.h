//! @file ActionHandler.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTCommunication/ActionDispatcherBase.h"

#include "OTCommunication/ActionDispatcher.h"
#include "OTCommunication/ActionHandleInlineConnector.h"
#include "OTCommunication/ActionHandleConnectorManager.h"

// std header
#include <functional>

namespace ot {

	class ActionDispatcherBase;
	class ActionHandleConnector;

	//! @brief The ActionHandler class may be used to simplify the management of multiple [\ref ActionHandleConnector "handlers"].
	//! The ActionHandler will register all created handlers at the provided [\ref ActionDispatcherBase "action dispatcher"] and will remove them when destroyed.
	//! If no dispatcher is provided, the global [\ref ActionDispatcher "action dispatcher"] will be used.
	class ActionHandler {
		OT_DECL_NOCOPY(ActionHandler)
	public:
		typedef std::function<std::string(JsonDocument&)> ActionHandlerMethodType;

		//! @brief Creates a new ActionHandler that will register all created [\ref ActionHandleConnector "handlers"] at the provided [\ref ActionDispatcherBase "action dispatcher"].
		//! @param _dispatcher The action dispatcher to register at. If nullptr is provided, the global [\ref ActionDispatcher "action dispatcher"] will be used.
		ActionHandler(ActionDispatcherBase* _dispatcher = &ot::ActionDispatcher::instance());

		//! @brief Move constructor.
		//! @param _other The other ActionHandler to move from.
		ActionHandler(ActionHandler&& _other) noexcept;
		~ActionHandler();

		//! @brief Move assignment operator.
		//! @param _other The other ActionHandler to move from.
		ActionHandler& operator=(ActionHandler&& _other) noexcept;

		//! @brief Creates a new [\ref ActionHandleConnector "handler"] and registers it at the currently set [\ref ActionDispatcherBase "action dispatcher"].
		//! The ActionHandler will take ownership of the created handler and will remove it from the dispatcher when destroyed.
		//! @tparam Function The type of the function to be called when a message is received.
		//! @param _actionName Name of the action to bind the handler to.
		//! @param _func The function to be called when a message is received.
		//! @param _messageFlags The message type flags the connector will accept.
		//! @param _insertFlags Flags to control the insert behavior at the [\ref ActionDispatcherBase "action dispatcher"].
		//! @return True if the handler was successfully registered at the dispatcher, false otherwise.
		template<typename Function>
		bool connectAction(const std::string& _actionName, Function&& _func, const ot::MessageTypeFlags& _messageFlags = ot::DEFAULT_MESSAGE_TYPE, const ActionDispatcherBase::InsertFlags& _insertFlags = ActionDispatcherBase::NoFlags);

		//! @brief Creates a new [\ref ActionHandleConnector "handler"] and registers it at the currently set [\ref ActionDispatcherBase "action dispatcher"].
		//! The ActionHandler will take ownership of the created handler and will remove it from the dispatcher when destroyed.
		//! @tparam Function The type of the function to be called when a message is received.
		//! @param _actionNames Names of the actions to bind the handler to.
		//! @param _func The function to be called when a message is received.
		//! @param _messageFlags The message type flags the connector will accept.
		//! @param _insertFlags Flags to control the insert behavior at the [\ref ActionDispatcherBase "action dispatcher"].
		//! @return True if the handler was successfully registered at the dispatcher, false otherwise.
		template<typename Function>
		bool connectAction(const std::initializer_list<std::string>& _actionNames, Function&& _func, const ot::MessageTypeFlags& _messageFlags = ot::DEFAULT_MESSAGE_TYPE, const ActionDispatcherBase::InsertFlags& _insertFlags = ActionDispatcherBase::NoFlags);

		//! @brief Creates a new [\ref ActionHandleConnector "handler"] and registers it at the currently set [\ref ActionDispatcherBase "action dispatcher"].
		//! The ActionHandler will take ownership of the created handler and will remove it from the dispatcher when destroyed.
		//! @tparam Function The type of the function to be called when a message is received.
		//! @param _actionNames Names of the actions to bind the handler to.
		//! @param _func The function to be called when a message is received.
		//! @param _messageFlags The message type flags the connector will accept.
		//! @param _insertFlags Flags to control the insert behavior at the [\ref ActionDispatcherBase "action dispatcher"].
		//! @return True if the handler was successfully registered at the dispatcher, false otherwise.
		template<typename Function>
		bool connectAction(const std::list<std::string>& _actionNames, Function&& _func, const ot::MessageTypeFlags& _messageFlags = ot::DEFAULT_MESSAGE_TYPE, const ActionDispatcherBase::InsertFlags& _insertFlags = ActionDispatcherBase::NoFlags);

		//! @brief Creates a new [\ref ActionHandleConnector "handler"] and registers it at the currently set [\ref ActionDispatcherBase "action dispatcher"].
		//! The ActionHandler will take ownership of the created handler and will remove it from the dispatcher when destroyed.
		//! @tparam T The class type of the instance the method belongs to.
		//! @tparam Method The type of the method to be called when a message is received.
		//! @param _actionName Name of the action to bind the handler to.
		//! @param _instance Pointer to the instance the method belongs to.
		//! @param _method The method to be called when a message is received.
		//! @param _messageFlags The message type flags the connector will accept.
		//! @param _insertFlags Flags to control the insert behavior at the [\ref ActionDispatcherBase "action dispatcher"].
		//! @return True if the handler was successfully registered at the dispatcher, false otherwise.
		template<typename T, typename Method>
		bool connectAction(const std::string& _actionName, T* _instance, Method&& _method, const ot::MessageTypeFlags& _messageFlags = ot::DEFAULT_MESSAGE_TYPE, const ActionDispatcherBase::InsertFlags& _insertFlags = ActionDispatcherBase::NoFlags);

		//! @brief Creates a new [\ref ActionHandleConnector "handler"] and registers it at the currently set [\ref ActionDispatcherBase "action dispatcher"].
		//! The ActionHandler will take ownership of the created handler and will remove it from the dispatcher when destroyed.
		//! @tparam T The class type of the instance the method belongs to.
		//! @tparam Method The type of the method to be called when a message is received.
		//! @param _actionNames Names of the actions to bind the handler to.
		//! @param _instance Pointer to the instance the method belongs to.
		//! @param _method The method to be called when a message is received.
		//! @param _messageFlags The message type flags the connector will accept.
		//! @param _insertFlags Flags to control the insert behavior at the [\ref ActionDispatcherBase "action dispatcher"].
		//! @return True if the handler was successfully registered at the dispatcher, false otherwise.
		template<typename T, typename Method>
		bool connectAction(const std::initializer_list<std::string>& _actionNames, T* _instance, Method&& _method, const ot::MessageTypeFlags& _messageFlags = ot::DEFAULT_MESSAGE_TYPE, const ActionDispatcherBase::InsertFlags& _insertFlags = ActionDispatcherBase::NoFlags);

		//! @brief Creates a new [\ref ActionHandleConnector "handler"] and registers it at the currently set [\ref ActionDispatcherBase "action dispatcher"].
		//! The ActionHandler will take ownership of the created handler and will remove it from the dispatcher when destroyed.
		//! @tparam T The class type of the instance the method belongs to.
		//! @tparam Method The type of the method to be called when a message is received.
		//! @param _actionNames Names of the actions to bind the handler to.
		//! @param _instance Pointer to the instance the method belongs to.
		//! @param _method The method to be called when a message is received.
		//! @param _messageFlags The message type flags the connector will accept.
		//! @param _insertFlags Flags to control the insert behavior at the [\ref ActionDispatcherBase "action dispatcher"].
		//! @return True if the handler was successfully registered at the dispatcher, false otherwise.
		template<typename T, typename Method>
		bool connectAction(const std::list<std::string>& _actionNames, T* _instance, Method&& _method, const ot::MessageTypeFlags& _messageFlags = ot::DEFAULT_MESSAGE_TYPE, const ActionDispatcherBase::InsertFlags& _insertFlags = ActionDispatcherBase::NoFlags);

	private:
		//! @brief Internal implementation of the connectAction methods.
		//! @param _actionNames Names of the actions to bind the handler to.
		//! @param _method The function to be called when a message is received.
		//! @param _messageFlags The message type flags the connector will accept.
		//! @param _insertFlags Flags to control the insert behavior at the [\ref ActionDispatcherBase "action dispatcher"].
		//! @return True if the handler was successfully registered at the dispatcher, false otherwise.
		bool connectActionImpl(const std::list<std::string>& _actionNames, const ActionHandlerMethodType& _method, const MessageTypeFlags& _messageFlags, const ActionDispatcherBase::InsertFlags& _insertFlags);

		std::list<ot::ActionHandleConnector*> m_handlers;
		ot::ActionDispatcherBase* m_dispatcher;
	};
}

















//! \brief Implementation of the inline connector setup.
//! Creates a new ot::ActionHandleInlineConnector instance and passes all arguments there.
//! The instance is accessable via the name "ActionHandleInlineConnector##___className##___functionName".
//! The generated object will look like this: <br>
//!		ot::ActionHandleInlineConnector<className> ActionHandleInlineConnector<className><functionName> { <br>
//!			<object>, <function>, <actionName>, <messageTypeFlags>, <connectorManager> <br>
//!		};
//! 
#define OT_CONNECT_HANDLE_IMPL(___object, ___className, ___function, ___functionName, ___actionName, ___messageTypeFlags, ___connectorManager) \
ot::ActionHandleInlineConnector<___className> ActionHandleInlineConnector##___className##___functionName { \
	___object, ___function, \
	___actionName, \
	___connectorManager, \
	___messageTypeFlags \
};

//! \brief Conveinienve layer to connect the action handler inline.
#define OT_CONNECT_HANDLE_CUSTOM(___functionName, ___className, ___actionName, ___messageTypeFlags, ___connectorManager) \
OT_CONNECT_HANDLE_IMPL(this, ___className, &___className::___functionName, ___functionName, ___actionName, ___messageTypeFlags, ___connectorManager);

//! \brief Conveinienve layer to connect the action handler inline.
//! Will use the default action handle connector manager.
#define OT_CONNECT_HANDLE(___functionName, ___className, ___actionName, ___messageTypeFlags) \
OT_CONNECT_HANDLE_CUSTOM(___functionName, ___className, ___actionName, ___messageTypeFlags, OT_DEFAULT_ACTION_HANDLER_NAME)

//! @brief Creates a function that will be registered in the dispatch system.
//! The generated function will look like this: <br>
//! 
//!		std::string <functionName>(JsonDocument& _document); <br>
//!		ot::ActionHandleInlineConnector<className> ActionHandleInlineConnector<className><functionName> { <br>
//!			<object>, <function>, <actionName>, <messageTypeFlags>, <connectorManager> <br>
//!		};
//! 
//! @param ___functionName The name of the new function.
//! @param ___className The name of the class where the function is created at.
//! @param ___actionName The action that should be registered in the dispatch system.
#define OT_HANDLER_CUSTOM(___functionName, ___className, ___actionName, ___messageTypeFlags, ___connectorManager) \
std::string ___functionName(ot::JsonDocument& _document); \
OT_CONNECT_HANDLE_CUSTOM(___functionName, ___className, ___actionName, ___messageTypeFlags, ___connectorManager);

//! @brief Creates a function that will be registered in the dispatch system.
//! Will use the default action handle connector manager. <br>
//! The generated function will look like this: <br>
//! 
//!		std::string <functionName>(JsonDocument& _document); <br>
//!		ot::ActionHandleInlineConnector<className> ActionHandleInlineConnector<className><functionName> { <br>
//!			<object>, <function>, <actionName>, <messageTypeFlags>, DefaultConnectorManager <br>
//!		};
//! 
//! @param ___functionName The name of the new function.
//! @param ___className The name of the class where the function is created at.
//! @param ___actionName The action that should be registered in the dispatch system.
#define OT_HANDLER(___functionName, ___className, ___actionName, ___messageTypeFlags) \
 OT_HANDLER_CUSTOM(___functionName, ___className, ___actionName, ___messageTypeFlags, OT_DEFAULT_ACTION_HANDLER_NAME)

//! \brief Creates a string list.
//! Use this whenever multiple actions should be connected to the same handler.
#define OT_ACTIONLIST(...) std::list<std::string>({__VA_ARGS__})

//! \def OT_DECL_ACTION_HANDLER
//! \brief Adds the default action handle connection management.
//! This should be placed in the private class declaration.
//! \ref ActionHandleConnectorManager
//! \param ___class The class 
//! \param ___dispatcher Will use the provided ActionDispatcherBase to register at.
#define OT_DECL_ACTION_HANDLER_CUSTOM(___class, ___dispatcher) \
private: \
	ot::ActionHandleConnectorManager<___class> OT_DEFAULT_ACTION_HANDLER_NAME{ this, ___dispatcher }; \
	void addActionHandler(ot::ActionHandleConnectorManager<___class>::HandlerMethodType _method, const std::string& _actionName, const ot::MessageTypeFlags& _messageTypes) { \
		OT_DEFAULT_ACTION_HANDLER_NAME.bindHandler(this, _method, _actionName, _messageTypes); \
	}

//! \def OT_DECL_ACTION_HANDLER
//! \brief Adds the default action handle connection management.
//! This should be placed at the beginning of the class declaration.
//! \ref ActionHandleConnectorManager
//! \param ___class The class
#define OT_DECL_ACTION_HANDLER(___class) OT_DECL_ACTION_HANDLER_CUSTOM(___class, &ot::ActionDispatcher::instance())

#include "OTCommunication/ActionHandler.hpp"