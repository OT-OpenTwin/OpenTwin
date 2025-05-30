//! @file Service.h
//! @authors Alexander Kuester (alexk95)
//! @date November 2020
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/CoreTypes.h"
#include "OTCore/Serializable.h"

// std header
#include <list>
#include <string>

class Session;

class Service : public ot::Serializable {
public:

	//! @brief Constructor.
	Service(const std::string& _url, const std::string& _name, ot::serviceID_t _id, const std::string& _type, Session* _session, bool _showDebugInfo);

	//! @brief Destructor.
	virtual ~Service();

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

	void setURL(const std::string& _url) { m_url = _url; };
	std::string getUrl(void) const { return m_url; };

	void setName(const std::string& _name) { m_name = _name; };
	std::string getName(void) const { return m_name; };

	void setID(ot::serviceID_t _id) { m_id = _id; };
	ot::serviceID_t getId(void) const { return m_id; };

	void setReceiveBroadcastMessages(bool _receive) { m_receiveBroadcastMessages = _receive; };
	bool getReceiveBroadcastMessages(void) const { return m_receiveBroadcastMessages; };

	std::string getType(void) const { return m_type; }

	Session* getSession(void) const { return m_session; };

	bool getIsVisible(void) const { return m_isVisible; };

	// ###########################################################################################################################################################################################################################################################################################################################

	// Getter

	//! @brief Will return the port numbers of this service
	virtual std::list<unsigned long long> getPortNumbers(void) const;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Serialization

	//! @brief Serialize the object data into the provided object by using the provided allocator.
	//! @param _object Json object value reference to write the data to.
	//! @param _allocator Allocator to use when writing data.
	//! @ref Serializable::addToJsonObject
	virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

	void addDebugInfoToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const;

	//! @brief Set the data by deserializing the object.
	//! Set the object contents from the provided JSON object.
	//! @param _object The JSON object containing the information.
	//! @ref Serializable::setFromJsonObject
	virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

protected:

	std::string				m_url;			//! This services IP address
	std::string				m_name;			//! This services name
	ot::serviceID_t			m_id;			//! This services ID
	std::string				m_type;			//! This services type
	Session *				m_session;		//! The session this service belongs to
	bool					m_receiveBroadcastMessages;
	bool					m_isVisible;

private:

	Service() = delete;
	Service(const Service &) = delete;
};
