//! \file AppBase.cpp
//! \author Alexander Kuester (alexk95)
//! \date January 2021
// ###########################################################################################################################################################################################################################################################################################################################

#include "AppBase.h"

#include "OTCore/ReturnMessage.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"

#include <thread>

#ifdef _DEBUG
#include <iostream>
#endif // _DEBUG

#define OT_LOG_BUFFER_LIMIT_LOW 1000
#define OT_LOG_BUFFER_LIMIT_MAX 10000

namespace intern {

	void replacePasswords(std::string& _string) {
		
		size_t passwordIx = _string.find(OT_ACTION_PASSWORD_SUBTEXT);
		size_t lastIx = 0; // Last index of previous match

		while (passwordIx != std::string::npos) {
			// Ensure the password is inside double quotes
			size_t checkIx = _string.find('\"', lastIx);
			if (checkIx > passwordIx) break;


			// Closing quote
			checkIx = _string.find('\"', passwordIx + 1);
			if (checkIx == std::string::npos) break;

			// Colon
			checkIx = _string.find(':', checkIx + 1);
			if (checkIx == std::string::npos) break;

			// Value opening quote
			checkIx = _string.find('\"', checkIx + 1);
			if (checkIx == std::string::npos) break;

			// Value closing quote
			lastIx = _string.find('\"', checkIx + 1);
			if (lastIx == std::string::npos) break;

			_string.replace(checkIx + 1, (lastIx - checkIx) - 1, "****");

			lastIx = _string.find('\"', checkIx + 1) + 1; // Last index is now invalid after replacing
			passwordIx = _string.find(OT_ACTION_PASSWORD_SUBTEXT, lastIx);
		}
	}
}

AppBase& AppBase::instance(void) {
	static AppBase g_instance;
	return g_instance;
}

void AppBase::log(const ot::LogMessage& _message) {
	this->appendLogMessage(ot::LogMessage(_message));
}

// ###########################################################################################################################################################################################################################################################################################################################

// Action handler

std::string AppBase::handleLog(ot::JsonDocument& _jsonDocument) {
	ot::ConstJsonObject obj = ot::json::getObject(_jsonDocument, OT_ACTION_PARAM_LOG);

	ot::LogMessage msg;
	msg.setFromJsonObject(obj);

	if (msg.getText().find(OT_ACTION_PASSWORD_SUBTEXT) != std::string::npos) {
		std::string newText = msg.getText();
		intern::replacePasswords(newText);
		msg.setText(newText);
	}

	msg.setCurrentTimeAsGlobalSystemTime();

	this->appendLogMessage(std::move(msg));

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string AppBase::handleRegister(ot::JsonDocument& _jsonDocument) {
	std::lock_guard<std::mutex> lock(m_mutex);

	std::string receiverUrl = ot::json::getString(_jsonDocument, OT_ACTION_PARAM_SERVICE_URL);

	m_receiver.push_back(receiverUrl);
#ifdef _DEBUG
	std::cout << "New Receiver: " << receiverUrl << std::endl;
#endif // _DEBUG

	ot::JsonDocument doc(rapidjson::kArrayType);
	for (ot::LogMessage& msg : m_messages) {
		ot::JsonObject msgObj;
		msg.addToJsonObject(msgObj, doc.GetAllocator());
		doc.PushBack(msgObj, doc.GetAllocator());
	}
	return doc.toJson();
}

std::string AppBase::handleDeregister(ot::JsonDocument& _jsonDocument) {
	std::lock_guard<std::mutex> lock(m_mutex);

	std::string receiverUrl = ot::json::getString(_jsonDocument, OT_ACTION_PARAM_SERVICE_URL);
	std::string ret = OT_ACTION_RETURN_VALUE_FAILED;
	auto it = std::find(m_receiver.begin(), m_receiver.end(), receiverUrl);
	if (it != m_receiver.end()) {
		m_receiver.erase(it);
		ret = OT_ACTION_RETURN_VALUE_OK;
#ifdef _DEBUG
		std::cout << "Removed Receiver: " << receiverUrl << std::endl;
#endif // _DEBUG
	}
	return ret;
}

std::string AppBase::handleClear(ot::JsonDocument& _jsonDocument) {
	std::lock_guard<std::mutex> newMessageLock(m_newMessageMutex);
	std::lock_guard<std::mutex> lock(m_mutex);

	m_messages.clear();
	m_count = 0;
	m_receiver.clear();
	m_newMessages.clear();

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string AppBase::handleGetDebugInfo(ot::JsonDocument& _jsonDocument) {
	std::lock_guard<std::mutex> newMessageLock(m_newMessageMutex);
	std::lock_guard<std::mutex> lock(m_mutex);

	ot::JsonDocument doc;
	doc.AddMember("MessageCount", m_count, doc.GetAllocator());
	doc.AddMember("NewMessageCount", m_newMessages.size(), doc.GetAllocator());
	doc.AddMember("Receivers", ot::JsonArray(m_receiver, doc.GetAllocator()), doc.GetAllocator());

	return doc.toJson();
}

std::string AppBase::handleSetGlobalLogFlags(ot::JsonDocument& _jsonDocument) {
	ot::ConstJsonArray flags = ot::json::getArray(_jsonDocument, OT_ACTION_PARAM_Flags);
	this->updateBufferSizeFromLogFlags(ot::logFlagsFromJsonArray(flags));

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string AppBase::handleSetCacheSize(ot::JsonDocument& _jsonDocument) {
	std::lock_guard<std::mutex> lock(m_mutex);

	size_t size = ot::json::getUInt64(_jsonDocument, OT_ACTION_PARAM_Size);
	m_bufferSize = size;
	this->resizeBuffer();

	return OT_ACTION_RETURN_VALUE_OK;
}

std::string AppBase::handleGetAllLogs(ot::JsonDocument& _jsonDocument) {
	ot::ReturnMessage response;

	std::lock_guard<std::mutex> lock(m_mutex);

	response = ot::ReturnMessage::Ok;

	ot::JsonDocument doc(rapidjson::kArrayType);

	for (const ot::LogMessage& msg : m_messages) {
		ot::JsonObject msgObj;
		msg.addToJsonObject(msgObj, doc.GetAllocator());
		doc.PushBack(msgObj, doc.GetAllocator());
	}

	response = doc.toJson();

	return response.toJson();
}

std::string AppBase::handleGetUserLogs(ot::JsonDocument& _jsonDocument) {
	ot::ReturnMessage response;

	std::string userName = ot::json::getString(_jsonDocument, OT_ACTION_PARAM_USER_NAME);

	if (userName.empty()) {
		response = ot::ReturnMessage::Failed;
		response = "User name is empty";
	}
	else {
		response = ot::ReturnMessage::Ok;
		
		ot::JsonDocument doc(rapidjson::kArrayType);

		std::lock_guard<std::mutex> lock(m_mutex);
		for (const ot::LogMessage& msg : m_messages) {
			if (msg.getUserName() == userName) {
				ot::JsonObject msgObj;
				msg.addToJsonObject(msgObj, doc.GetAllocator());
				doc.PushBack(msgObj, doc.GetAllocator());
			}
		}

		response = doc.toJson();
	}

	return response.toJson();
}

void AppBase::updateBufferSizeFromLogFlags(const ot::LogFlags& _flags) {
	std::lock_guard<std::mutex> lock(m_mutex);

	if ((_flags | ot::WARNING_LOG | ot::ERROR_LOG) == (ot::WARNING_LOG | ot::ERROR_LOG)) {
		m_bufferSize = OT_LOG_BUFFER_LIMIT_LOW;
	}
	else {
		m_bufferSize = OT_LOG_BUFFER_LIMIT_MAX;
	}
	OT_LOG_D("Log buffer size set to " + std::to_string(m_bufferSize));

	this->resizeBuffer();
}

// ###########################################################################################################################################################################################################################################################################################################################

// Private: Helper

void AppBase::appendLogMessage(ot::LogMessage&& _message) {
	std::lock_guard<std::mutex> lock(m_newMessageMutex);

	m_newMessages.push_back(std::move(_message));
}

void AppBase::workerNotify() {
	while (m_notifyThreadRunning) {
		std::list<ot::LogMessage> messagesToSend;
		{
			std::lock_guard<std::mutex> newLock(m_newMessageMutex);
			messagesToSend = std::move(m_newMessages);
			m_newMessages.clear();
		}

		if (!messagesToSend.empty()) {
			// Prepare the message to be sent
			ot::JsonDocument doc;
			doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_Log, doc.GetAllocator()), doc.GetAllocator());

			size_t newCount = 0;

			ot::JsonArray messagesArray;
			for (const ot::LogMessage& msg : messagesToSend) {
				messagesArray.PushBack(ot::JsonObject(msg, doc.GetAllocator()), doc.GetAllocator());
				m_fileManager.appendLog(msg);
				newCount++;
			}
			m_fileManager.flush();

			doc.AddMember(OT_ACTION_PARAM_LOGS, messagesArray, doc.GetAllocator());
			std::string msgStr = doc.toJson();

			// Notify all receivers
			std::list<std::string> failed;
			std::lock_guard<std::mutex> mainLock(m_mutex);

			for (auto& receiver : m_receiver) {
				std::string response;
				if (!ot::msg::send("", receiver, ot::EXECUTE, msgStr, response, ot::msg::defaultTimeout, ot::msg::DefaultFlagsNoExit)) {
					failed.push_back(receiver);
				}
			}

			// Add the new messages to the main message list
			m_messages.splice(m_messages.end(), std::move(messagesToSend));
			messagesToSend.clear();

			m_count += newCount;

			// Shrink buffer if necessary
			while (m_count > m_bufferSize) {
				m_messages.pop_front();
				m_count--;
			}
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(100));

	}
}

void AppBase::removeReceiver(const std::string& _receiver) {
	bool erased = true;
	while (erased) {
		erased = false;
		for (auto it = m_receiver.begin(); it != m_receiver.end(); it++) {
			if (*it == _receiver) {
				m_receiver.erase(it);
				erased = true;
				break;
			}
		}
	}

#ifdef _DEBUG
	std::cout << "Receiver removed: " << _receiver << std::endl;
#endif // _DEBUG
}

void AppBase::resizeBuffer() {
	if (m_count > m_bufferSize) {
		auto it = m_messages.begin();
		std::advance(it, m_count - m_bufferSize);
		m_messages.erase(m_messages.begin(), it);
		m_count = m_bufferSize;
	}
}

AppBase::AppBase() 
	: ot::ServiceBase(OT_INFO_SERVICE_TYPE_LOGGER, OT_INFO_SERVICE_TYPE_LOGGER), m_count(0), m_bufferSize(1000), m_notifyThreadRunning(false)
{
	// Set this so the log dispatcher will not destroy the AppBase
	this->setCustomDeleteLogNotifier(true);

	m_fileManager.initialize();

	m_notifyThreadRunning = true;
	m_notifyThread = new std::thread(&AppBase::workerNotify, this);
}

AppBase::~AppBase() {
	m_notifyThreadRunning = false;

	if (m_notifyThread) {
		if (m_notifyThread->joinable()) {
			m_notifyThread->join();
		}
		delete m_notifyThread;
		m_notifyThread = nullptr;
	}
}