/*
 *	msg.cpp
 *
 *  Created on: January 03, 2021
 *	Author: Alexander Kuester
 *  Copyright (c) 2020 openTwin
 */

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCore/OTAssert.h"
#include "OTCommunication/Msg.h"			// Corresponding header
#include "OTCommunication/ActionTypes.h"

// Curl header
#include "curl/curl.h"

#include <thread>
#include <chrono>
#include <cassert>
#include <filesystem>
#include <iostream>
#include <fstream>

namespace ot {
	namespace intern {
		void sendAsyncWorker(const std::string& _senderURL, const std::string& _receiverURL, ot::MessageType _type, const std::string& _message, int _timeout);
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

bool ot::msg::send(
	const std::string &				_senderIP,
	const std::string &				_receiverIP,
	MessageType						_type,
	const std::string &				_message,
	std::string &					_response,
	int								_timeout,
	bool							_shutdownMessage,
	bool							_createLogMessage
) {
	if (_senderIP == _receiverIP) {
		if (_createLogMessage) { OT_LOG_W("Receiver is the same as sender: \"" + _receiverIP + "\". Ignoring message..."); }
		return true;
	}

	if (_createLogMessage) { OT_LOG("Sending message to (Receiver = \"" + _receiverIP + "\"; Endpoint = " + (_type == ot::EXECUTE ? "Execute" : (_type == ot::QUEUE ? "Queue" : "Execute one way TLS")) + "). Message = \"" + _message + "\"", ot::OUTGOING_MESSAGE_LOG); }

	//using namespace std::chrono_literals;
	//thread_local static bool lock = false;
	//while (lock) std::this_thread::sleep_for(1ms);
	//lock = true;

	// Setting up the shared data between handles.
	thread_local static CURLSH* share = nullptr;

	if (!_shutdownMessage)
	{
		if (share == nullptr)
		{
			share = curl_share_init();
			curl_share_setopt(share, CURLSHOPT_SHARE, CURL_LOCK_DATA_COOKIE);
			curl_share_setopt(share, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);
			curl_share_setopt(share, CURLSHOPT_SHARE, CURL_LOCK_DATA_SSL_SESSION); // Share SSL Cache
			curl_share_setopt(share, CURLSHOPT_SHARE, CURL_LOCK_DATA_CONNECT); // Share Connection Cache
		}
	}
	else
	{
		if (share != nullptr)
		{
			curl_share_cleanup(share);
			share = nullptr;
		}
	}

	auto curl = curl_easy_init();
	if (!curl)
	{
		//lock = false;
		OTAssert(0, "Failed to initialize curl");
		OT_LOG_E("Failed to initialize curl");
		return false;
	}

	//NOTE, is the url the receiver url?
	std::string curlURL("https://");
	curlURL.append(_receiverIP).append("/");

	switch (_type)
	{
	case QUEUE: curlURL.append("queue"); break;
	case EXECUTE: curlURL.append("execute"); break;
	case EXECUTE_ONE_WAY_TLS: curlURL.append("execute-one-way-tls"); break;
	default:
		//lock = false;
		OTAssert(0, "Unknown message type");
		OT_LOG_E("Unknown message type");
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

	if (_type == EXECUTE_ONE_WAY_TLS)
	{
		if (ca_cert_file.empty())
		{
			// Get the path of the executable (
#ifdef _WIN32
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

			// Chyeck whether local cert file ca.pem exists
			if (!std::filesystem::exists(ca_cert_file))
			{
				// Get the development root environment variable and build the path to the deployment cert file
				std::string dev_root = get_env_var("OPENTWIN_DEV_ROOT");
				ca_cert_file = dev_root + "\\Deployment\\Certificates\\ca.pem";
			}
		}

		// cannot log here: will deadlock... find a better solution to add this to log
		//OT_LOG_D("Using CA certificate file: " + ca_cert_file);

		curl_easy_setopt(curl, CURLOPT_CAINFO, ca_cert_file.c_str());
	}
	else
	{
		// If the environment variables are set, then we perform a call through mTLS

		if (ca_client_cert.empty() || ca_client_key.empty() || ca_cert_file.empty())
		{
			ca_cert_file = get_env_var("OPEN_TWIN_CA_CERT");
			ca_client_cert = get_env_var("OPEN_TWIN_SERVER_CERT");
			ca_client_key = get_env_var("OPEN_TWIN_SERVER_CERT_KEY");
		}

		if (ca_client_cert != "" && ca_client_key != "" && ca_cert_file != "")
		{
			curl_easy_setopt(curl, CURLOPT_SSLCERT, ca_client_cert.c_str());
			curl_easy_setopt(curl, CURLOPT_SSLKEY, ca_client_key.c_str());
			curl_easy_setopt(curl, CURLOPT_CAINFO, ca_cert_file.c_str());
		}
	}

	curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, (long)CURL_HTTP_VERSION_2TLS);
	curl_easy_setopt(curl, CURLOPT_VERBOSE, 0); // For testing purposes set to 1

	if (!_shutdownMessage)
	{
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
	//std::cout << "URL Code received: " << res << std::endl;

	curl_easy_cleanup(curl);
	//curl_share_cleanup(share);
	curl = NULL;

	//lock = false;
	if (errorCode == CURLE_OK)
	{
		if (_createLogMessage) 
		{ 
			OT_LOG(".. Message sent successful (Receiver = \"" + _receiverIP + "\"; Endpoint = " + (_type == ot::EXECUTE ? "Execute" : (_type == ot::QUEUE ? "Queue" : "Execute one way TLS")) + "). Response = \"" + _response + "\"", ot::OUTGOING_MESSAGE_LOG); 
		}
		return true;
	}
	else
	{
		std::string errorString = curl_easy_strerror(errorCode);
		if (_createLogMessage)
		{
			OT_LOG_D(
				".. Message sent failed "
				"Error message: " + errorString + "; "
				"Error buffer: " + errbuf + "; "
				"(Receiver = \"" + _receiverIP + "\"; "
				"Endpoint = " + (_type == ot::EXECUTE ? "Execute" : (_type == ot::QUEUE ? "Queue" : "Execute one way TLS")) + "). );"
				
			);
		}
		return false;
	}
}

void ot::msg::sendAsync(
	const std::string &				_senderIP,
	const std::string &				_receiverIP,
	MessageType						_type,
	const std::string &				_message,
	int								_timeout
) {
	std::thread t(ot::intern::sendAsyncWorker, _senderIP, _receiverIP, _type, _message, _timeout);
	t.detach();
}

void ot::intern::sendAsyncWorker(const std::string& _senderIP, const std::string& _receiverIP, ot::MessageType _type, const std::string& _message, int _timeout) {
	std::string response;
	if (!ot::msg::send(_senderIP, _receiverIP, _type, _message, response, _timeout)) {
		OT_LOG_E("[ASYNC] Failed to send message to \"" + _receiverIP + "\"");
	}
	OT_ACTION_IF_RESPONSE_ERROR(response) {
		OT_LOG_E("[ASYNC] " + response);
	}
	OT_ACTION_IF_RESPONSE_WARNING(response) {
		OT_LOG_W("[ASYNC] " + response);
	}
}