#pragma once

// OpenTwin header
#include "OTServiceFoundation/FoundationAPIExport.h"
#include "OTCore/rJSON.h"
#include "OTCommunication/CommunicationTypes.h"

// std header
#include <string>
#include <map>
#include <list>
#include <mutex>

#pragma warning(disable : 4251)

namespace ot {

	class AbstractDispatchItem;

	class OT_SERVICEFOUNDATION_API_EXPORT Dispatcher {
	public:
		// todo: rework to reference instead of pointer
		static Dispatcher * instance(void);
		static Dispatcher * uiPluginInstance(void);
		static void deleteAllInstances(void);
		static void deleteInstance(void);
		static void deleteUiPluginInstance(void);

		void add(AbstractDispatchItem * _item);
		void remove(AbstractDispatchItem * _item);

		//! @brief This function may be called from the main external API that is receiving the perform/ queue messages
		//! This function will forward the call to the dispatch function and will create a C-String copy of the result so
		//! the string can be directy returned out of the dll/application
		const char * dispatchWrapper(const char * _json, const char * _senderUrl, ot::MessageType _messageType);

		//! @brief Will get and forward the action to all registered handlers
		std::string dispatch(const std::string& _json, ot::MessageType _messageType);

		//! @brief Will forward the action to all registered handlers
		//! Will return the result of the last handler
		//! Uses a mutex to ensure that only one request is handled at a time
		//! @param _action The action to dispatch
		//! @param _document The document containing the action parameter
		std::string dispatch(const std::string& _action, OT_rJSON_doc& _document, ot::MessageType _messageType);

		//! @brief Will forward the action to all registered handlers
		//! Will return the result of the last handler
		//! Uses a mutex to ensure that only one request is handled at a time
		//! @param _action The action to dispatch
		//! @param _document The document containing the action parameter
		//! @param _handlerFound Will be set to true if at least one handler was found to dispatch this action to
		std::string dispatch(const std::string& _action, OT_rJSON_doc& _document, bool& _handlerFound, ot::MessageType _messageType);

	private:

		std::map<std::string, std::list<AbstractDispatchItem *> *>	m_data;
		std::mutex													m_mutex;

		Dispatcher();
		virtual ~Dispatcher();
		Dispatcher(Dispatcher&) = delete;
		Dispatcher& operator = (Dispatcher&) = delete;
	};
}