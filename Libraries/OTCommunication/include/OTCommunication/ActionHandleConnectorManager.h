// @otlicense

//! @file ActionHandleBuilder.h
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/CoreTypes.h"
#include "OTCommunication/CommunicationTypes.h"
#include "OTCommunication/ActionDispatcherBase.h"

// std header
#include <list>
#include <string>
#include <functional>

namespace ot {

	class ActionHandleConnector;

	//! @brief The ActionHandleConnectorManager is used for action handle connector management of an object.
	//! @tparam T Class type.
	template <class T> class ActionHandleConnectorManager {
		OT_DECL_NOCOPY(ActionHandleConnectorManager)
		OT_DECL_NOMOVE(ActionHandleConnectorManager)
		OT_DECL_NODEFAULT(ActionHandleConnectorManager)
	public:
		ActionHandleConnectorManager(T* _obj, ActionDispatcherBase* _dispatcher);
		virtual ~ActionHandleConnectorManager();

		typedef std::string(T::*HandlerMethodType)(JsonDocument& _document);

		virtual void bindHandler(T* _object, HandlerMethodType _method, const std::string& _actionName, const MessageTypeFlags& _messageFlags);
		virtual void bindHandler(T* _object, HandlerMethodType _method, const std::list<std::string>& _actionNames, const MessageTypeFlags& _messageFlags);

	private:
		T* m_obj;
		ActionDispatcherBase* m_dispatcher;
		std::list<ActionHandleConnector*> m_connectors;
	};

}

#include "OTCommunication/ActionHandleConnectorManager.hpp"