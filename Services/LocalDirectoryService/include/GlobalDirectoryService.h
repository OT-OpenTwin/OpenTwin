// @otlicense

//! @file GlobalDirectoryService.h
//! @author Alexander Kuester (alexk95)
//! @date September 2022
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/ServiceBase.h"

// std header
#include <string>

class GlobalDirectoryService : public ot::ServiceBase {
public:
	enum ConnectionStatus {
		Connected,
		Disconnected,
		WaitingForConnection
	};

	GlobalDirectoryService(void);
	virtual ~GlobalDirectoryService();
	
	// ###########################################################################################################################################################################################################################################################################################################################

	// Connection

	bool isConnected(void) const { return m_connectionStatus == Connected; };
	ConnectionStatus connectionStatus(void) const { return m_connectionStatus; };

	//! @brief Will set the service URL and start a worker thread to register at the GDS.
	//! The connection status will be set to WaitingForConnection until the registration is completed.
	//! @param _url GDS url to set.
	void connect(const std::string& _url);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Worker methods

private:
	//! @brief Worker thread entry.
	//! Will send a register request to the GDS.
	//! If the registration fails the application will exit.
	void registerAtGlobalDirectoryService(void);

	//! @brief Health check function.
	//! Will be called by the register method after successful registration.
	//! In case that the GDS crashed the Application::globalDirectoryServiceCrashed method will be called.
	void healthCheck(void);

	//! @brief Will add the current system values to the provided JSON document.
	void addSystemValues(ot::JsonDocument& _jsonDocument);

	bool				m_isShuttingDown;
	ConnectionStatus	m_connectionStatus;

	GlobalDirectoryService(GlobalDirectoryService&) = delete;
	GlobalDirectoryService& operator = (GlobalDirectoryService&) = delete;
};