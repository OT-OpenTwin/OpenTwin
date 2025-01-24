//! @file ActionHandler.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTCommunication/ActionDispatcher.h"
#include "OTCommunication/ActionHandleInlineConnector.h"
#include "OTCommunication/ActionHandleConnectorManager.h"

//! \brief Implementation of the inline connector setup.
//! Creates a new ot::ActionHandleInlineConnector instance and passes all arguments there.
//! The instance is accessable via the name "ActionHandleInlineConnector##___className##___functionName".
//! //! The generated object will look like this: <br>
//! 
//!		ot::ActionHandleInlineConnector<className> ActionHandleInlineConnector<className><functionName> { <br>
//!			<object>, <function>, <actionName>, <messageTypeFlags>, <connectorManager> <br>
//!		};
//! 
#define OT_CONNECT_HANDLE_IMPL(___object, ___className, ___function, ___functionName, ___actionName, ___messageTypeFlags, ___connectorManager) \
ot::ActionHandleInlineConnector<___className> ActionHandleInlineConnector##___className##___functionName { \
	___object, ___function, \
	___actionName, ___messageTypeFlags, \
	___connectorManager  \
};

//! 
//!		ot::ActionHandleInlineConnector<className> ActionHandleInlineConnector<className><functionName> { <br>
//!			<object>, <function>, <actionName>, <messageTypeFlags>, <connectorManager> <br>
//!		};
//! 
#define OT_CONNECT_HANDLE_IMPL(___object, ___className, ___function, ___functionName, ___actionName, ___messageTypeFlags, ___connectorManager) \
ot::ActionHandleInlineConnector<___className> ActionHandleInlineConnector##___className##___functionName { \
	___object, ___function, \
	___actionName, ___messageTypeFlags, \
	___connectorManager  \
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

