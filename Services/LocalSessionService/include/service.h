//! @file Service.h
//! @authors Alexander Kuester (alexk95)
//! @date November 2020
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/Flags.h"
#include "OTCore/CoreTypes.h"
#include "OTCore/Serializable.h"
#include "OTCore/OTClassHelper.h"

// std header
#include <list>
#include <string>

class Session;

class Service : public ot::Serializable {
	OT_DECL_NOCOPY(Service)
	OT_DECL_NODEFAULT(Service)
public:
	//! @brief Service flags.
	enum ServiceFlag {
		NoServiceFlags           = 0 << 0, //! @brief No service flags.
		IsVisible                = 1 << 0, //! @brief The service is visible and known to other services.
		IsShuttingDown           = 1 << 1, //! @brief The service is in a shutdown state and is expected to disconnect soon.
		UseRelay                 = 1 << 2, //! @brief The services uses a relay service for communication (e.g. UI).
		ReceiveBroadcastMessages = 1 << 3  //! @brief The service will receive broadcast messages.
	};
	typedef ot::Flags<ServiceFlag> ServiceFlags;

	//! @brief Constructor.
	Service(const std::string& _url, const std::string& _name, ot::serviceID_t _id, const std::string& _type, const std::string& _sessionId, const ServiceFlags& _flags = ServiceFlag::NoServiceFlags);

	Service(Service&& _other) noexcept;

	//! @brief Destructor.
	virtual ~Service();

	Service& operator = (Service&& _other) noexcept;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Management

	//! @brief Set the visible state of the service to true.
	//! Setting a service to visible will notify all other services in the same session.
	void setVisible(void);

	//! @brief Set the visible state of the service to false.
	//! Setting a service to hidden will notify all other services in the same session.
	void setHidden(void);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Setter / Getter

	void setServiceURL(const std::string& _url) { m_url = _url; };
	std::string getServiceUrl(void) const { return m_url; };

	void setServiceName(const std::string& _name) { m_name = _name; };
	std::string getServiceName(void) const { return m_name; };

	void setServiceID(ot::serviceID_t _id) { m_id = _id; };
	ot::serviceID_t getServiceId(void) const { return m_id; };

	std::string getServiceType(void) const { return m_type; };

	void setRelayUrl(const std::string& _url) { m_relayUrl = _url; };
	const std::string& getRelayUrl(void) const { return m_relayUrl; };

	void setWebsocketUrl(const std::string& _url) { m_websocketUrl = _url; };
	const std::string& getWebsocketUrl(void) const { return m_websocketUrl; };

	std::string getSessionId(void) const { return m_sessionId; };

	bool getIsVisible(void) const { return m_flags.flagIsSet(ServiceFlag::IsVisible); };

	void setUseRelay(bool _useRelay) { m_flags.setFlag(ServiceFlag::UseRelay, _useRelay); };
	bool getUseRelay(void) const { return m_flags.flagIsSet(ServiceFlag::UseRelay); };

	void setReceiveBroadcastMessages(bool _receive) { m_flags.setFlag(ServiceFlag::ReceiveBroadcastMessages, _receive); };
	bool getReceiveBroadcastMessages(void) const { return m_flags.flagIsSet(ServiceFlag::ReceiveBroadcastMessages); };

	//! @brief Will return the port numbers of this service
	std::list<unsigned long long> getPortNumbers(void) const;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Serialization

	//! @brief Serialize the object data into the provided object by using the provided allocator.
	//! @param _object Json object value reference to write the data to.
	//! @param _allocator Allocator to use when writing data.
	//! @ref Serializable::addToJsonObject
	virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

	//! @brief Adds the full service information to the provided json object.
	//! @ref Service::addToJsonObject
	//! @param _object Json object value reference to write the data to.
	//! @param _allocator Allocator to use when writing data.
	void addDebugInfoToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const;

	//! @brief Set the data by deserializing the object.
	//! Set the object contents from the provided JSON object.
	//! @param _object The JSON object containing the information.
	//! @ref Serializable::setFromJsonObject
	virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

private:
	std::string	    m_url;          //! @brief Service url.
	std::string	    m_name;         //! @brief Service name.
	ot::serviceID_t	m_id;           //! @brief Service ID.
	std::string	    m_type;         //! @brief Service type
	std::string     m_relayUrl;     //! @brief Websocket url.
	std::string     m_websocketUrl; //! @brief Websocket url.
	std::string     m_sessionId;    //! @brief ID of session this services is running in.

	ServiceFlags    m_flags;        //! @brief Flags.
};

OT_ADD_FLAG_FUNCTIONS(Service::ServiceFlag)