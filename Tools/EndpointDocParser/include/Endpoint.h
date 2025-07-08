#pragma once

// project header
#include "Parameter.h"

// std header
#include <string>
#include <list>

class Endpoint {
public:
	enum MessageType {
		mTLS,
		TLS
	};

	Endpoint() = default;
	Endpoint(const Endpoint& _other) = default;
	Endpoint(Endpoint&& _other) noexcept = default;

	~Endpoint() = default;

	Endpoint& operator = (const Endpoint& _other) = default;
	Endpoint& operator = (Endpoint&& _other) noexcept = default;

	void setName(const std::string& _name) { m_name = _name; };
	const std::string& getName() const { return m_name; };

	void setAction(const std::string& _action) { m_action = _action; };
	const std::string& getAction() const { return m_action; };

	void setBriefDescription(const std::string& _briefDescription) { m_briefDescription = _briefDescription; };
	const std::string& getBriefDescription() const { return m_briefDescription; };

	void setDetailedDescription(const std::string& _detailedDescription) { m_detailedDescription = _detailedDescription; };
	const std::string& getDetailedDescription() const { return m_detailedDescription; };

	void setMessageType(MessageType _messageType) { m_messageType = _messageType; };
	MessageType getMessageType() const { return m_messageType; };
	std::string getMessageTypeString() const;

	void setParameters(const std::list<Parameter>& _parameters) { m_parameters = _parameters; };
	const std::list<Parameter>& getParameters() const { return m_parameters; };

	void setResponseDescription(const std::string& _responseDescription) { m_responseDescription = _responseDescription; };
	const std::string& getResponseDescription() const { return m_responseDescription; };

	void setResponseParameters(const std::list<Parameter>& _responseParameters) { m_responseParameters = _responseParameters; };
	const std::list<Parameter>& getResponseParameters() const { return m_responseParameters; };

	void addParameter(const Parameter& _parameter) { m_parameters.push_back(_parameter); };
	void addResponseParameter(const Parameter& _parameter) { m_responseParameters.push_back(_parameter); };

	void printEndpoint() const;

private:
	std::string m_name;
	std::string m_action;
	std::string m_briefDescription;
	std::string m_detailedDescription;
	MessageType m_messageType;
	std::list<Parameter> m_parameters;
	std::string m_responseDescription;
	std::list<Parameter> m_responseParameters;
};