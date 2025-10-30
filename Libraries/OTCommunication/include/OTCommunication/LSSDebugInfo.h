// @otlicense

#pragma once

// OpenTwin header
#include "OTSystem/SystemTypes.h"
#include "OTCore/CoreTypes.h"
#include "OTCore/Serializable.h"
#include "OTCommunication/CommunicationAPIExport.h"

namespace ot {

	class OT_COMMUNICATION_API_EXPORT LSSDebugInfo : public ot::Serializable {
		OT_DECL_DEFCOPY(LSSDebugInfo)
		OT_DECL_DEFMOVE(LSSDebugInfo)
	public:
		struct ServiceInfo {
			ot::serviceID_t id = 0;
			std::string name;
			std::string type;
			std::string url;
			std::string websocketUrl;
			std::string connectedType;
			
			bool isDebug = false;
			bool isRequested = false;
			bool isAlive = false;
			bool isRunning = false;
			bool isShuttingDown = false;
			bool isHidden = false;
		};

		struct SessionInfo {
			std::string id;
			std::string type;

			std::string userName;
			std::string projectName;
			std::string collectionName;

			std::string userCollection;

			std::string userCredName;
			std::string userCredPassword;

			std::string dataBaseCredName;
			std::string dataBaseCredPassword;

			std::list<ServiceInfo> services;

			bool isHealthCheckRunning = false;
			bool isShuttingDown = false;
		};

		LSSDebugInfo() = default;
		virtual ~LSSDebugInfo() = default;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Virtual methods

		virtual void addToJsonObject(ot::JsonValue& _jsonObject, ot::JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ot::ConstJsonObject& _jsonObject) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setWorkerRunning(bool _running) { m_workerRunning = _running; };
		bool getWorkerRunning() const { return m_workerRunning; };

		void setURL(const std::string& _url) { m_url = _url; };
		const std::string& getURL() const { return m_url; };

		void setID(ot::serviceID_t _id) { m_ID = _id; };
		ot::serviceID_t getID() const { return m_ID; };

		void setGSSUrl(const std::string& _url) { m_gssUrl = _url; };
		const std::string& getGSSUrl() const { return m_gssUrl; };

		void setGSSConnected(bool _connected) { m_gssConnected = _connected; };
		bool getGSSConnected() const { return m_gssConnected; };

		void setGDSUrl(const std::string& _url) { m_gdsUrl = _url; };
		const std::string& getGDSUrl() const { return m_gdsUrl; };

		void setGDSConnected(bool _connected) { m_gdsConnected = _connected; };
		bool getGDSConnected() const { return m_gdsConnected; };

		void setAuthUrl(const std::string& _url) { m_authUrl = _url; };
		const std::string& getAuthUrl() const { return m_authUrl; };

		void setDataBaseUrl(const std::string& _url) { m_dataBaseUrl = _url; };
		const std::string& getDataBaseUrl() const { return m_dataBaseUrl; };

		void setLMSUrl(const std::string& _url) { m_lmsUrl = _url; };
		const std::string& getLMSUrl() const { return m_lmsUrl; };

		void addDebugService(const std::string& _service) { m_debugServices.push_back(_service); };
		void setDebugServices(const std::list<std::string>& _services) { m_debugServices = _services; };
		const std::list<std::string>& getDebugServices() const { return m_debugServices; };

		void addMandatoryService(const std::string& _sessionType, const std::list<std::string>& _services) { m_mandatoryServices.push_back(std::make_pair(_sessionType, _services)); };
		const std::list<std::pair<std::string, std::list<std::string>>>& getMandatoryServices() const { return m_mandatoryServices; };

		void addToShutdownQueue(const std::string& _service, bool _isEmergency) { m_shutdownQueue.push_back(std::make_pair(_service, _isEmergency)); };
		void setShutdownQueue(const std::list<std::pair<std::string, bool>>& _queue) { m_shutdownQueue = _queue; };
		const std::list<std::pair<std::string, bool>>& getShutdownQueue() const { return m_shutdownQueue; };

		void addToShutdownCompletedQueue(const std::string& _service) { m_shutdownCompletedQueue.push_back(_service); };
		void setShutdownCompletedQueue(const std::list<std::string>& _queue) { m_shutdownCompletedQueue = _queue; };
		const std::list<std::string>& getShutdownCompletedQueue() const { return m_shutdownCompletedQueue; };

		void addBlockedPort(port_t _port) { m_blockedPorts.push_back(_port); };
		void setBlockedPorts(const std::list<port_t>& _ports) { m_blockedPorts = _ports; };
		const std::list<port_t>& getBlockedPorts() const { return m_blockedPorts; };

		void addSession(const SessionInfo& _session) { m_sessions.push_back(_session); };
		void addSession(SessionInfo&& _session) { m_sessions.push_back(std::move(_session)); };
		void setSessions(const std::list<SessionInfo>& _sessions) { m_sessions = _sessions; };
		const std::list<SessionInfo>& getSessions() const { return m_sessions; };

	private:
		bool m_workerRunning = false;

		std::string m_url;
		ot::serviceID_t m_ID = 0;

		std::string m_gssUrl;
		bool m_gssConnected = false;

		std::string m_gdsUrl;
		bool m_gdsConnected = false;

		std::string m_authUrl;
		std::string m_dataBaseUrl;
		std::string m_lmsUrl;

		std::list<std::string> m_debugServices;
		std::list<std::pair<std::string, std::list<std::string>>> m_mandatoryServices;

		std::list<std::pair<std::string, bool>> m_shutdownQueue;
		std::list<std::string> m_shutdownCompletedQueue;

		std::list<port_t> m_blockedPorts;

		std::list<SessionInfo> m_sessions;
	};
}