// @otlicense

#pragma once
#include <mutex>
#include "OTCore/Json.h"
#include "OTCore/ServiceBase.h"
#include "ApplicationBase.h"

class MessageQueueHandler
{
public:
	static MessageQueueHandler& instance()
	{
		static MessageQueueHandler instance;
		return instance;
	}
	
	~MessageQueueHandler() = default;
	MessageQueueHandler& operator=(MessageQueueHandler&& _other) = delete;
	MessageQueueHandler& operator=(const MessageQueueHandler& _other) = delete;

	void flushIfAppropriate(int _queueRequestIncrement, ot::ServiceBase* _serviceBase);

	bool addToQueueIfRequired(ot::ServiceBase* _serviceBase, const std::string& _projectName, const ot::JsonDocument& _doc, std::list<std::pair<ot::UID, ot::UID>>& _prefetchIds);

	void clearServiceBuffer(ot::ServiceBase* _serviceBase);

	void flushServiceBuffer(ot::ServiceBase* _serviceBase);

	void setOwnServiceURL(const std::string _ownServiceURL) { m_ownServiceURL = _ownServiceURL; }
private:
	MessageQueueHandler() = default;

	struct MessageBuffer
	{
		int m_queueRequestCounter = 0;
		ot::JsonDocument* m_compoundDocument = nullptr;
		std::recursive_mutex m_queueRequestMtx;
	};
	
	std::map<std::string, MessageBuffer> m_messageBufferByServiceName;	
	std::string m_ownServiceURL = "";
	std::mutex m_bufferInitiationMtx;


	MessageBuffer& getMessageBuffer(ot::ServiceBase* _service);
	ot::JsonDocument* getInitiatedCompoundDocument(ot::ServiceBase* _serviceBase, const std::string& _projectName);
	void addMessageToQueue(ot::ServiceBase* _serviceInfo, const std::string& _projectName, const ot::JsonDocument& _doc, std::list<std::pair<ot::UID, ot::UID>>& _prefetchIds);
};
