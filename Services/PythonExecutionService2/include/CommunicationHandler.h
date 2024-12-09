#pragma once

// OpenTwin header
#include "OTCore/JSON.h"
#include "OTCore/Flags.h"
#include "OTCore/OTClassHelper.h"

// Qt header
#include <QtNetwork/qlocalserver.h>

class SubprocessManager;

class CommunicationHandler : public QLocalServer {
	Q_OBJECT
	OT_DECL_NOCOPY(CommunicationHandler)
	OT_DECL_NODEFAULT(CommunicationHandler)
public:
	CommunicationHandler(SubprocessManager* _manager, const std::string& _serverName);
	~CommunicationHandler();

	bool sendToClient(const ot::JsonDocument& _document, std::string& _response);

	const std::string& getServerName(void) const { return m_serverName; };

private Q_SLOTS:
	void slotNewConnection(void);
	void slotMessageReceived(void);
	void slotClientDisconnected(void);

private:
	enum class ClientState {
		Disconnected,
		WaitForPing,
		Ready,
		WaitForResponse,
		ReponseReceived
	};

	void processNextEvent(void);

	bool waitForClient(void);
	bool sendToClient(const QByteArray& _data, std::string& _response);

	SubprocessManager* m_manager;
	std::string m_serverName;

	QLocalSocket* m_client;
	ClientState m_clientState;

	std::string m_response;

};