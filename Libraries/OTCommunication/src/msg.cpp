//! @file Msg.h
//! @authors Alexander Kuester, Peter Thoma
//! @date January 2021
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTSystem/AppExitCodes.h"
#include "OTSystem/OperatingSystem.h"
#include "OTSystem/ArchitectureInfo.h"
#include "OTCore/Logger.h"
#include "OTCore/String.h"
#include "OTCore/BasicScopedBoolWrapper.h"
#include "OTCore/ThisComputerInfo.h"
#include "OTCommunication/Msg.h"			// Corresponding header
#include "OTCommunication/ActionTypes.h"

// Curl header
#include "curl/curl.h"

// std header
#include <mutex>
#include <thread>
#include <chrono>
#include <cassert>
#include <filesystem>
#include <iostream>
#include <fstream>

static std::string g_lastError;
std::mutex g_errorState;

namespace ot {
	namespace intern {

		//! @class TimeoutManager
		//! @brief The TimeoutManager is used to handle the default timeout in case no timeout was provided to a send method.
		//! The "global default timeout" is loaded once from the environment and buffered.
		class TimeoutManager {
			OT_DECL_NOCOPY(TimeoutManager)
			OT_DECL_NOMOVE(TimeoutManager)
		public:
			static int getTimeout(void) {
				static TimeoutManager g_instance;

				if (g_instance.m_timeout <= 0) {
					static std::atomic_bool readingEnv{ false };
					if (readingEnv) {
						while (readingEnv) {};
					}
					else {
						readingEnv = true;

						// Read timeout from env
						bool failed = true;
						const std::string envEntry = ThisComputerInfo::getEnvEntry(ThisComputerInfo::EnvMessageTimeout);
						
						if (envEntry.empty()) {
							g_instance.m_timeout = msg::defaultTimeout;
							OT_LOG_I("Setting global default timeout to " + std::to_string(msg::defaultTimeout) + " since no timeout was set in the environment");
						}
						else {
							g_instance.m_timeout = ot::String::toNumber<int>(envEntry, failed);

							// Ensure valid timeout is set
							if (failed) {
								g_instance.m_timeout = msg::defaultTimeout;
								OT_LOG_E("Failed to convert global timeout from environment: \"" + envEntry + "\"");
							}
							else if (g_instance.m_timeout <= 0) {
								g_instance.m_timeout = msg::defaultTimeout;
								OT_LOG_I("Setting global default timeout to " + std::to_string(msg::defaultTimeout) + " since environment timeout is 0");
							}
						}

						readingEnv = false;
					}
				}

				return g_instance.m_timeout;
			}

		private:
			TimeoutManager() : m_timeout(0) {};
			~TimeoutManager() {};

			std::atomic_int m_timeout;
		};

		void sendAsyncWorker(const std::string& _senderURL, const std::string& _receiverURL, ot::MessageType _type, const std::string& _message, int _timeout, const ot::msg::RequestFlags& _flags);
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



const std::string ot::msg::getLastError()
{
	std::lock_guard<std::mutex> guard(g_errorState);
	return g_lastError;
}

bool ot::msg::send(const std::string& _senderIP, const std::string& _receiverIP, MessageType _type, const std::string& _message, std::string& _response, int _timeout, const RequestFlags& _flags) {
	// Ensure receiver url was provided
	if (_receiverIP.empty()) {
		if (_flags & msg::CreateLogMessage) {
			OT_LOG_W("Receiver url is empty. Ignoring message...");
		}
		return false;
	}

	// Block explicit self message (allow by providing empty sender)
	if (_senderIP == _receiverIP) {
		if (_flags & msg::CreateLogMessage) {
			OT_LOG_W("Receiver is the same as sender: \"" + _receiverIP + "\". Ignoring message...");
		}
		return true;
	}

	// If log message should be generated, do so now
	if (_flags & msg::CreateLogMessage) {
		OT_LOG("Sending message to (Receiver = \"" + _receiverIP + "\"; Endpoint = " + (_type == ot::EXECUTE ? "Execute" : (_type == ot::QUEUE ? "Queue" : "Execute one way TLS")) + "). Message = \"" + _message + "\"", ot::OUTGOING_MESSAGE_LOG);
	}

	// Update timeout if needed
	if (_timeout <= 0) {
		_timeout = intern::TimeoutManager::getTimeout();
	}

	// Setting up the shared data between handles.
	thread_local static CURLSH* share = nullptr;

	if (_flags & msg::IsShutdownMessage) {
		if (share != nullptr) {
			curl_share_cleanup(share);
			share = nullptr;
		}
	}
	else {
		if (share == nullptr) {
			share = curl_share_init();
			curl_share_setopt(share, CURLSHOPT_SHARE, CURL_LOCK_DATA_COOKIE);
			curl_share_setopt(share, CURLSHOPT_SHARE, CURL_LOCK_DATA_DNS);
			curl_share_setopt(share, CURLSHOPT_SHARE, CURL_LOCK_DATA_SSL_SESSION); // Share SSL Cache
			curl_share_setopt(share, CURLSHOPT_SHARE, CURL_LOCK_DATA_CONNECT); // Share Connection Cache
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

	if (!(_flags & msg::IsShutdownMessage)) {
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
		if (_flags & msg::CreateLogMessage)  {
			OT_LOG(".. Message sent successful (Receiver = \"" + _receiverIP + "\"; Endpoint = " + (_type == ot::EXECUTE ? "Execute" : (_type == ot::QUEUE ? "Queue" : "Execute one way TLS")) + "). Response = \"" + _response + "\"", ot::OUTGOING_MESSAGE_LOG); 
		}
		return true;
	}
	else {
		// Store last error as error string
		g_lastError = "{ \"Error message\": \"" + std::string(curl_easy_strerror(errorCode)) +
			"\", \"Error buffer\": \"" + errbuf +
			"\", \"Receiver\": \"" + _receiverIP +
			"\", \"Endpoint\": " + (_type == ot::EXECUTE ? "\"Execute\"" : (_type == ot::QUEUE ? "\"Queue\"" : "\"Execute one way TLS\"")) +
			" }";

		// If log message should be generated, do so now
		if (_flags & msg::CreateLogMessage) {
			OT_LOG_E("Message sent failed: " + g_lastError );
		}

		// If exit on fail is set, assert and exit
		if (_flags & msg::ExitOnFail) {
			OTAssert(0, "Failed to send message. Exiting...");
			exit(ot::AppExitCode::SendFailed);
		}

		return false;
	}
}

void ot::msg::sendAsync(const std::string& _senderIP, const std::string& _receiverIP, MessageType _type, const std::string& _message, int _timeout, const RequestFlags& _flags) {
	std::thread t(ot::intern::sendAsyncWorker, _senderIP, _receiverIP, _type, _message, _timeout, _flags);
	t.detach();
}

void ot::intern::sendAsyncWorker(const std::string& _senderIP, const std::string& _receiverIP, ot::MessageType _type, const std::string& _message, int _timeout, const ot::msg::RequestFlags& _flags) {
	std::string response;
	if (!ot::msg::send(_senderIP, _receiverIP, _type, _message, response, _timeout, _flags)) {
		OT_LOG_E("[ASYNC] Failed to send message to \"" + _receiverIP + "\"");
	}
	OT_ACTION_IF_RESPONSE_ERROR(response) {
		OT_LOG_E("[ASYNC] " + response);
	}
	OT_ACTION_IF_RESPONSE_WARNING(response) {
		OT_LOG_W("[ASYNC] " + response);
	}
}