/*
 *	msg.cpp
 *
 *  Created on: January 03, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2020 openTwin
 */

// OpenTwin header
#include "OTSystem/OperatingSystem.h"
#include "OTSystem/ArchitectureInfo.h"
#include "OTCore/Logger.h"
#include "OTCore/String.h"
#include "OTCore/OTAssert.h"
#include "OTCore/ThisComputerInfo.h"
#include "OTCommunication/Msg.h"			// Corresponding header
#include "OTCommunication/ActionTypes.h"

// Curl header
#include "curl/curl.h"

#include <mutex>
#include <thread>
#include <chrono>
#include <cassert>
#include <filesystem>
#include <iostream>
#include <fstream>

namespace ot {
	namespace intern {

		class TimeoutManager {
			OT_DECL_NOCOPY(TimeoutManager)
			OT_DECL_NOMOVE(TimeoutManager)
		public:
			static int getTimeout(void) {
				static std::mutex g_mutex;
				std::lock_guard<std::mutex> lock(g_mutex);

				static TimeoutManager g_instance;
				if (g_instance.m_timeout <= 0) {
					// Read timeout from env
					bool failed = true;
					g_instance.m_timeout = ot::String::toNumber<int>(ThisComputerInfo::getEnvEntry(ThisComputerInfo::EnvMessageTimeout), failed);
					// Ensure valid timeout is set
					if (failed || g_instance.m_timeout <= 0) {
						g_instance.m_timeout = msg::defaultTimeout;
					}
				}

				return g_instance.m_timeout;
			}

		private:
			TimeoutManager() : m_timeout(0) {};
			~TimeoutManager() {};

			int m_timeout;
		};

		void sendAsyncWorker(const std::string& _senderURL, const std::string& _receiverURL, ot::MessageType _type, const std::string& _message, int _timeout, bool _createLogMessage);
	}
}

size_t writeFunction(void *ptr, size_t size, size_t nmemb, std::string* data) {
	data->append((char*)ptr, size * nmemb);
	return size * nmemb;
}

std::string get_env_var(std::string const& key)
{
	char buffer[4096];
	size_t environmentVariableValueStringLength;

	getenv_s(&environmentVariableValueStringLength, buffer,
		sizeof(buffer) - 1, key.c_str());

	return std::string(buffer);
}

bool ot::msg::send(const std::string& _senderIP, const std::string& _receiverIP, MessageType _type, const std::string& _message, std::string& _response, int _timeout, bool _shutdownMessage, bool _createLogMessage) {
	// Block explicit self message (allow by providing empty sender)
	if (_senderIP == _receiverIP) {
		if (_createLogMessage) {
			OT_LOG_W("Receiver is the same as sender: \"" + _receiverIP + "\". Ignoring message...");
		}
		return true;
	}

	// If log message should be generated, do so now
	if (_createLogMessage) {
		OT_LOG("Sending message to (Receiver = \"" + _receiverIP + "\"; Endpoint = " + (_type == ot::EXECUTE ? "Execute" : (_type == ot::QUEUE ? "Queue" : "Execute one way TLS")) + "). Message = \"" + _message + "\"", ot::OUTGOING_MESSAGE_LOG);
	}

	// By default a send fail will simply return false.
	// If using the default timeout exit will be called on send fail.
	bool shutdownOnSendFail = false;

	// Update timeout if needed
	if (_timeout <= 0) {
		_timeout = intern::TimeoutManager::getTimeout();
		shutdownOnSendFail = true;
	}

	// Setting up the shared data between handles.
	thread_local static CURLSH* share = nullptr;

	if (!_shutdownMessage) {
		if (share == nullptr) {
			share = curl_share_init();
			curl_share_setopt(share, CURLSHOPT_SHARE, CURL_LOCK_DATA_COOKIE);
			curl_share_setopt(share, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);
			curl_share_setopt(share, CURLSHOPT_SHARE, CURL_LOCK_DATA_SSL_SESSION); // Share SSL Cache
			curl_share_setopt(share, CURLSHOPT_SHARE, CURL_LOCK_DATA_CONNECT); // Share Connection Cache
		}
	}
	else {
		if (share != nullptr) {
			curl_share_cleanup(share);
			share = nullptr;
		}
	}

	auto curl = curl_easy_init();
	if (!curl) {
		OT_LOG_EA("Failed to initialize curl");
		return false;
	}

	std::string curlURL("https://");
	curlURL.append(_receiverIP).append("/");

	switch (_type) {
	case QUEUE: curlURL.append("queue"); break;
	case EXECUTE: curlURL.append("execute"); break;
	case EXECUTE_ONE_WAY_TLS: curlURL.append("execute-one-way-tls"); break;
	default:
		OT_LOG_EA("Unknown message type");
		return false;
	}

	struct curl_slist *headers = NULL;
	headers = curl_slist_append(headers, "Accept: application/json");
	headers = curl_slist_append(headers, "Content-Type: application/json");
	headers = curl_slist_append(headers, "charset: utf-8");

	curl_easy_setopt(curl, CURLOPT_URL, curlURL.c_str());
	curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
	curl_easy_setopt(curl, CURLOPT_POSTFIELDS, _message.c_str());
	//curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
	//curl_easy_setopt(curl, CURLOPT_NOPROGRESS, 1L);
	//curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 50L);
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcrp/0.1");
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, _timeout);

	std::string header_string;
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, writeFunction);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, &_response);
	curl_easy_setopt(curl, CURLOPT_HEADERDATA, &header_string);

	// SSL/TLS configuration
	static std::string ca_cert_file;
	static std::string ca_client_cert;
	static std::string ca_client_key;

	if (_type == EXECUTE_ONE_WAY_TLS) {
		if (ca_cert_file.empty()) {
			ca_cert_file = get_env_var("OPEN_TWIN_CA_CERT");

			if (!std::filesystem::exists(ca_cert_file)) {
				// Get the path of the executable 
#ifdef OT_OS_WINDOWS
				char path[MAX_PATH] = { 0 };
				GetModuleFileNameA(NULL, path, MAX_PATH);
				ca_cert_file = path;
#else
				char result[PATH_MAX];
				ssize_t count = readlink("/proc/self/exe", result, PATH_MAX);
				ca_cert_flile = std::string(result, (count > 0) ? count : 0);
#endif
				ca_cert_file = ca_cert_file.substr(0, ca_cert_file.rfind('\\'));
				ca_cert_file += "\\Certificates\\ca.pem";

				// Check whether local cert file ca.pem exists
				if (!std::filesystem::exists(ca_cert_file)) {
					// Get the development root environment variable and build the path to the deployment cert file
					std::string dev_root = get_env_var("OPENTWIN_DEV_ROOT");
					ca_cert_file = dev_root + "\\Deployment\\Certificates\\ca.pem";
				}
			}
		}

		curl_easy_setopt(curl, CURLOPT_CAINFO, ca_cert_file.c_str());
	}
	else {
		// If the environment variables are set, then we perform a call through mTLS
		if (ca_client_cert.empty() || ca_client_key.empty() || ca_cert_file.empty()) {
			ca_cert_file = get_env_var("OPEN_TWIN_CA_CERT");
			ca_client_cert = get_env_var("OPEN_TWIN_SERVER_CERT");
			ca_client_key = get_env_var("OPEN_TWIN_SERVER_CERT_KEY");
		}

		if (ca_client_cert != "" && ca_client_key != "" && ca_cert_file != "") {
			curl_easy_setopt(curl, CURLOPT_SSLCERT, ca_client_cert.c_str());
			curl_easy_setopt(curl, CURLOPT_SSLKEY, ca_client_key.c_str());
			curl_easy_setopt(curl, CURLOPT_CAINFO, ca_cert_file.c_str());
		}
	}

	curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 0); // For testing purposes set to 1

	if (!_shutdownMessage) {
		curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1L);

		// SHARED CACHES CONFIG
		curl_easy_setopt(curl, CURLOPT_SHARE, share);
	}
	
	/* provide a buffer to store errors in */
	char errbuf[CURL_ERROR_SIZE];
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);
	errbuf[0] = 0;

	// Send the request
	CURLcode errorCode = curl_easy_perform(curl);
	
	curl_easy_cleanup(curl);
	curl = NULL;

	if (errorCode == CURLE_OK) {
		if (_createLogMessage)  {
			OT_LOG(".. Message sent successful (Receiver = \"" + _receiverIP + "\"; Endpoint = " + (_type == ot::EXECUTE ? "Execute" : (_type == ot::QUEUE ? "Queue" : "Execute one way TLS")) + "). Response = \"" + _response + "\"", ot::OUTGOING_MESSAGE_LOG); 
		}
		return true;
	}
	else {
		if (_createLogMessage) {
			OT_LOG_E("Message sent failed: { \"Error message\": \"" + std::string(curl_easy_strerror(errorCode)) +
				"\", \"Error buffer\": \"" + errbuf + 
				"\", \"Receiver\": \"" + _receiverIP +
				"\", \"Endpoint\": " + (_type == ot::EXECUTE ? "\"Execute\"" : (_type == ot::QUEUE ? "\"Queue\"" : "\"Execute one way TLS\"")) +
				" }");
		}

		if (shutdownOnSendFail) {
			exit(1);
		}

		return false;
	}
}

void ot::msg::sendAsync(
	const std::string &				_senderIP,
	const std::string &				_receiverIP,
	MessageType						_type,
	const std::string &				_message,
	int								_timeout,
	bool							_createLogMessage
) {
	std::thread t(ot::intern::sendAsyncWorker, _senderIP, _receiverIP, _type, _message, _timeout, _createLogMessage);
	t.detach();
}

void ot::intern::sendAsyncWorker(const std::string& _senderIP, const std::string& _receiverIP, ot::MessageType _type, const std::string& _message, int _timeout, bool _createLogMessage) {
	std::string response;
	if (!ot::msg::send(_senderIP, _receiverIP, _type, _message, response, _timeout, false, _createLogMessage)) {
		OT_LOG_E("[ASYNC] Failed to send message to \"" + _receiverIP + "\"");
	}
	OT_ACTION_IF_RESPONSE_ERROR(response) {
		OT_LOG_E("[ASYNC] " + response);
	}
	OT_ACTION_IF_RESPONSE_WARNING(response) {
		OT_LOG_W("[ASYNC] " + response);
	}
}