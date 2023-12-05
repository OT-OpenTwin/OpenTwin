#pragma once

// OpenTwin header
#include "OTCore/Flags.h"	// Flags
#include "OTCore/JSON.h" // rJSON doc/val
#include "OTCommunication/CommunicationTypes.h" // MessageType type
#include "OTServiceFoundation/FoundationAPIExport.h" // API export

// std header
#include <functional>
#include <string>

#pragma warning(disable : 4251)

namespace ot {

	class OT_SERVICEFOUNDATION_API_EXPORT OTObject {
	public:
		OTObject() {}
		virtual ~OTObject() {}
	};

	class OT_SERVICEFOUNDATION_API_EXPORT AbstractDispatchItem {
	public:
		AbstractDispatchItem(const std::string& _actionName, const ot::Flags<ot::MessageType>& _messageFlags);
		virtual ~AbstractDispatchItem();

		bool mayDispatch(ot::MessageType _inboundMessageType) const;

		// todo: add endpoint (execute, queue ...) to dispatch and ensure the handler does allow the enpoint
		// if the user adds a handler then he needs to specify which endpoints are allowed
		virtual std::string forwardDispatch(JsonDocument& _document) = 0;

		const std::string& actionName(void) const { return m_actionName; }

	protected:
		std::string					m_actionName;
		ot::Flags<ot::MessageType>	m_messageFlags;
	};

	template <class T> class __declspec(dllexport) OTHandleConnector : public AbstractDispatchItem {
	public:
		typedef std::string(T::*ConnectorMessageRef)(JsonDocument&);

		OTHandleConnector();
		OTHandleConnector(const std::string& _actionName, const ot::Flags<ot::MessageType>& _messageFlags, OTObject * _obj, ConnectorMessageRef _func);
		OTHandleConnector(const OTHandleConnector<T>& _other);
		OTHandleConnector& operator = (const OTHandleConnector& _other);

		OTObject * object(void) { return m_obj; }
		ConnectorMessageRef dispatchReference(void) { return m_func; }

		virtual std::string forwardDispatch(JsonDocument& _doc) override;

	private:
		OTObject *					m_obj;
		ConnectorMessageRef			m_func;
	};

}

template <class T>
ot::OTHandleConnector<T>::OTHandleConnector(void) : AbstractDispatchItem(std::string(), ot::ALL_MESSAGE_TYPES), m_obj(nullptr), m_func(nullptr) {}

template <class T>
ot::OTHandleConnector<T>::OTHandleConnector(const std::string& _actionName, const ot::Flags<ot::MessageType>& _messageFlags, OTObject * _obj, ConnectorMessageRef _func) : AbstractDispatchItem(_actionName, _messageFlags), m_obj(_obj), m_func(_func) {}

template <class T>
ot::OTHandleConnector<T>::OTHandleConnector(const OTHandleConnector<T>& _other) : AbstractDispatchItem(_other.m_actionName, _other.m_messageFlags), m_obj(_other.m_obj), m_func(_other.m_func) {}

template <class T>
ot::OTHandleConnector<T>& ot::OTHandleConnector<T>::operator = (const OTHandleConnector& _other) {
	m_actionName = _other.m_actionName;
	m_messageFlags = _other.m_messageFlags;
	m_obj = _other.m_obj;
	m_func = _other.m_func;
	return *this;
}

template <class T>
std::string ot::OTHandleConnector<T>::forwardDispatch(JsonDocument& _doc) {
	if (m_func) return std::invoke(m_func, dynamic_cast<T *>(m_obj), _doc);
	else return std::string();
}

#define OT_ADD_HANDLE(___functionName, ___className, ___actionName, ___messageTypeFlags) ot::OTHandleConnector<___className> othandleconnector##___functionName##=ot::OTHandleConnector<___className>(___actionName, ___messageTypeFlags, this, &___className::___functionName)

//! @brief Creates a function that will be registered in the dispatch system
//! The generated function will look like this
//!		std::string <functionName>(JsonDocument& _document);
//! @param ___functionName The name of the new function
//! @param ___className The name of the class where the function is created at
//! @param ___actionName The action that should be registered in the dispatch system
#define OT_HANDLER(___functionName, ___className, ___actionName, ___messageTypeFlags) std::string ___functionName(ot::JsonDocument& _document); OT_ADD_HANDLE(___functionName, ___className, ___actionName, ___messageTypeFlags);