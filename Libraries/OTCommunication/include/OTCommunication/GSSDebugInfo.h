// @otlicense

//! @file GSSDebugInfo.h
//! @author Alexander Kuester (alexk95)
//! @date August 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/CoreTypes.h"
#include "OTCore/Serializable.h"
#include "OTCommunication/CommunicationAPIExport.h"

// std header
#include <list>

namespace ot {

	class OT_COMMUNICATION_API_EXPORT GSSDebugInfo : public ot::Serializable {
		OT_DECL_DEFCOPY(GSSDebugInfo)
		OT_DECL_DEFMOVE(GSSDebugInfo)
	public:
		struct SessionData {
			std::string sessionID;
			std::string userName;
			std::list<std::string> flags;
		};

		struct LSSData {
			ot::serviceID_t id;
			std::string url;
			std::list<SessionData> activeSessions;
			std::list<SessionData> initializingSessions;
		};

		GSSDebugInfo() = default;
		virtual ~GSSDebugInfo() = default;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Virtual methods

		virtual void addToJsonObject(ot::JsonValue& _jsonObject, ot::JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ot::ConstJsonObject& _jsonObject) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setUrl(const std::string& _url) { m_url = _url; };
		const std::string& getUrl() const { return m_url; };

		void setDatabaseUrl(const std::string& _url) { m_databaseUrl = _url; };
		const std::string& getDatabaseUrl() const { return m_databaseUrl; };

		void setAuthorizationUrl(const std::string& _url) { m_authorizationUrl = _url; };
		const std::string& getAuthorizationUrl() const { return m_authorizationUrl; };

		void setGlobalDirectoryUrl(const std::string& _url) { m_globalDirectoryUrl = _url; };
		const std::string& getGlobalDirectoryUrl() const { return m_globalDirectoryUrl; };

		void setLibraryManagementUrl(const std::string& _url) { m_libraryManagementUrl = _url; };
		const std::string& getLibraryManagementUrl() const { return m_libraryManagementUrl; };

		void setWorkerRunning(bool _running) { m_workerRunning = _running; };
		bool isWorkerRunning() const { return m_workerRunning; };

		void addLSS(LSSData&& _data) { m_lssList.push_back(std::move(_data)); };
		const std::list<LSSData>& getLSSList() const { return m_lssList; };

		void addSessionToLSSEntry(const std::string& _sessionID, ot::serviceID_t _lssID) { m_sessionToLssList.push_back(std::make_pair(_sessionID, _lssID)); };
		const std::list<std::pair<std::string, ot::serviceID_t>>& getSessionToLssList() const { return m_sessionToLssList; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private: Helper

	private:
		void addToJsonObject(ot::JsonValue& _jsonObject, ot::JsonAllocator& _allocator, const LSSData& _data) const;
		void setFromJsonObject(const ot::ConstJsonObject& _jsonObject, LSSData& _data);

		void addToJsonObject(ot::JsonValue& _jsonObject, ot::JsonAllocator& _allocator, const SessionData& _data) const;
		void setFromJsonObject(const ot::ConstJsonObject& _jsonObject, SessionData& _data);

		std::string m_url;
		std::string m_databaseUrl;
		std::string m_authorizationUrl;
		std::string m_globalDirectoryUrl;
		std::string m_libraryManagementUrl;
		
		bool m_workerRunning = true;

		std::list<std::pair<std::string, ot::serviceID_t>> m_sessionToLssList;
		std::list<LSSData> m_lssList;
	};
}