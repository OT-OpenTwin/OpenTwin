// @otlicense

#include "OTCommunication/ActionTypes.h"
#include "OTServiceFoundation/MessageQueueHandler.h"

void MessageQueueHandler::flushIfAppropriate(int _queueRequestIncrement, ot::ServiceBase* _serviceBase)
{
	MessageBuffer& buffer = getMessageBuffer(_serviceBase);
	std::lock_guard<std::recursive_mutex> guard(buffer.m_queueRequestMtx);
	buffer.m_queueRequestCounter += _queueRequestIncrement;
	
	if (buffer.m_queueRequestCounter < 0) {
		//OTAssert(0, "Enabled counter reached a negative value"); 
		// Can happen. If a manual flush is being applied then the counter is set to zero because of a flush, but any enabled locks may still try to unlock
		buffer.m_queueRequestCounter = 0;
		return;
	}

	if (buffer.m_queueRequestCounter == 0)
	{
		flushServiceBuffer(_serviceBase);
	}
}

bool MessageQueueHandler::addToQueueIfRequired(ot::ServiceBase* _serviceBase, const std::string& _projectName, const ot::JsonDocument& _doc, std::list<std::pair<ot::UID, ot::UID>>& _prefetchIds)
{
	MessageBuffer& buffer = getMessageBuffer(_serviceBase);
	//Required since the document may be added to the buffer in addMessageToQueue and the m_queueRequestCounter has to be guarded as well.
	std::lock_guard<std::recursive_mutex> guard(buffer.m_queueRequestMtx);

	bool queueRequired = false;
	if (buffer.m_queueRequestCounter > 0)
	{
		queueRequired = true;
		addMessageToQueue(_serviceBase, _projectName, _doc, _prefetchIds);
	}

	return queueRequired;
}

void MessageQueueHandler::clearServiceBuffer(ot::ServiceBase* _serviceBase)
{
	MessageBuffer& buffer = getMessageBuffer(_serviceBase);
	std::lock_guard<std::recursive_mutex> guard(buffer.m_queueRequestMtx);
	if (buffer.m_compoundDocument != nullptr)
	{
		delete buffer.m_compoundDocument;
		buffer.m_compoundDocument = nullptr;
	}

	buffer.m_queueRequestCounter = 0;
}


void MessageQueueHandler::flushServiceBuffer(ot::ServiceBase* _serviceBase)
{
	MessageBuffer& buffer = getMessageBuffer(_serviceBase);
	
	std::unique_ptr<ot::JsonDocument>compoundDocument(nullptr);
	
	{
		std::lock_guard<std::recursive_mutex> guard(buffer.m_queueRequestMtx);
		compoundDocument.reset(buffer.m_compoundDocument);
		buffer.m_compoundDocument = nullptr;
		clearServiceBuffer(_serviceBase);
	}

	if (compoundDocument != nullptr)
	{
		std::string response;
		if (compoundDocument->IsObject()) {

			const bool success = ot::msg::send(m_ownServiceURL, _serviceBase->getServiceURL(), ot::QUEUE, compoundDocument->toJson(), response);
			if (!success)
			{
				assert(false);
				OT_LOG_E("Failed to flush compound message");
			}
		}
		else
		{
			OTAssert(0, "The queued doc is not a object");
		}

	}
	else
	{
		OT_LOG_D("Tried to flush messages to service: " + _serviceBase->getServiceName() + " but no message was buffered.");
	}
}

MessageQueueHandler::MessageBuffer& MessageQueueHandler::getMessageBuffer(ot::ServiceBase* _service)
{
	auto messageBufferByServiceName = m_messageBufferByServiceName.find(_service->getServiceName());
	if (messageBufferByServiceName == m_messageBufferByServiceName.end())
	{
		std::lock_guard<std::mutex> guard(m_bufferInitiationMtx);
		m_messageBufferByServiceName.try_emplace(_service->getServiceName());
		messageBufferByServiceName = m_messageBufferByServiceName.find(_service->getServiceName());
	}
	return messageBufferByServiceName->second;
}

ot::JsonDocument* MessageQueueHandler::getInitiatedCompoundDocument(ot::ServiceBase* _serviceBase, const std::string& _projectName)
{
	MessageBuffer& buffer = getMessageBuffer(_serviceBase);
	
	if (buffer.m_compoundDocument == nullptr)
	{
		buffer.m_compoundDocument = new ot::JsonDocument;

		ot::JsonArray docs;
		ot::JsonArray prefetchedIDs;
		ot::JsonArray prefetchedVersions;

		buffer.m_compoundDocument->AddMember(OT_ACTION_MEMBER, OT_ACTION_CMD_Compound, buffer.m_compoundDocument->GetAllocator());
		buffer.m_compoundDocument->AddMember(OT_ACTION_PARAM_PROJECT_NAME, ot::JsonString(_projectName, buffer.m_compoundDocument->GetAllocator()), buffer.m_compoundDocument->GetAllocator());
		buffer.m_compoundDocument->AddMember(OT_ACTION_PARAM_PREFETCH_Documents, docs, buffer.m_compoundDocument->GetAllocator());
		buffer.m_compoundDocument->AddMember(OT_ACTION_PARAM_PREFETCH_ID, prefetchedIDs, buffer.m_compoundDocument->GetAllocator());
		buffer.m_compoundDocument->AddMember(OT_ACTION_PARAM_PREFETCH_Version, prefetchedVersions, buffer.m_compoundDocument->GetAllocator());
	}

	return buffer.m_compoundDocument;
}

void MessageQueueHandler::addMessageToQueue(ot::ServiceBase* _serviceInfo, const std::string& _projectName, const ot::JsonDocument& _doc, std::list<std::pair<ot::UID, ot::UID>>& _prefetchIds)
{

	//Neither this nor getInitiatedCompoundDocument have additional locks. Only addToQueueIfRequired is calling addMessageToQueue and addToQueueIfRequired has a lock
	ot::JsonDocument* queuedDoc = MessageQueueHandler::instance().getInitiatedCompoundDocument(_serviceInfo, _projectName);
	
	ot::JsonAllocator& allocator = queuedDoc->GetAllocator();
	ot::JsonValue messageDocument(_doc, allocator);
	(*queuedDoc)[OT_ACTION_PARAM_PREFETCH_Documents].PushBack(messageDocument, allocator);

	for (auto id : _prefetchIds)
	{
		(*queuedDoc)[OT_ACTION_PARAM_PREFETCH_ID].PushBack(id.first, allocator);
		(*queuedDoc)[OT_ACTION_PARAM_PREFETCH_Version].PushBack(id.second, allocator);
	}
}
