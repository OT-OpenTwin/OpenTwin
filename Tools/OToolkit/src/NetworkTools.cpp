//! @file NetworkTools.cpp
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "NetworkTools.h"

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCore/StringHelper.h"
#include "OTWidgets/Label.h"
#include "OTWidgets/ComboBox.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/PlainTextEdit.h"

// Qt header
#include <QtWidgets/qwidget.h>
#include <QtWidgets/qlayout.h>
#include <QtNetwork/qtcpserver.h>

#define NETWORKTOOLS_PORTTYPE_ANY "Any"
#define NETWORKTOOLS_PORTTYPE_IPv4 "IPv4"
#define NETWORKTOOLS_PORTTYPE_IPv6 "IPv6"

NetworkTools::NetworkTools()
	: m_portBlockerPorts(nullptr), m_runPortBlockerButton(nullptr), m_portBlockerType(nullptr)
{

}

NetworkTools::~NetworkTools() {

}

bool NetworkTools::runTool(QMenu* _rootMenu, otoolkit::ToolWidgets& _content) {
	// PORT BLOCKER
	QWidget* portBlockerLayoutW = new QWidget;
	QGridLayout* portBlockerLayout = new QGridLayout(portBlockerLayoutW);

	m_portBlockerPorts = new ot::PlainTextEdit;
	ot::Label* portBlockerLabel = new ot::Label("Ports to block:");
	m_runPortBlockerButton = new ot::PushButton("Run");
	
	m_portBlockerType = new ot::ComboBox;
	m_portBlockerType->addItems({ NETWORKTOOLS_PORTTYPE_ANY, NETWORKTOOLS_PORTTYPE_IPv4, NETWORKTOOLS_PORTTYPE_IPv6 });
	m_portBlockerType->setCurrentIndex(0);

	portBlockerLayout->addWidget(m_portBlockerType, 0, 0);
	portBlockerLayout->addWidget(portBlockerLabel, 1, 0);
	portBlockerLayout->addWidget(m_portBlockerPorts, 2, 0);
	portBlockerLayout->addWidget(m_runPortBlockerButton, 2, 1, Qt::AlignCenter);
	portBlockerLayout->setRowStretch(2, 1);
	portBlockerLayout->setColumnStretch(0, 1);

	ot::WidgetView* portBlockerView = this->createCentralWidgetView(portBlockerLayoutW, "Port Blocker");
	_content.addView(portBlockerView);

	this->connect(m_runPortBlockerButton, &QPushButton::clicked, this, &NetworkTools::slotRunPortBlocker);

	return true;
}

void NetworkTools::restoreToolSettings(QSettings& _settings) {
	m_portBlockerPorts->setPlainText(_settings.value("NET.BlockedPorts", QString()).toString());
	
	QString blockType = _settings.value("NET.BlockedPortsType", NETWORKTOOLS_PORTTYPE_ANY).toString();
	if (blockType == NETWORKTOOLS_PORTTYPE_IPv4) m_portBlockerType->setCurrentText(NETWORKTOOLS_PORTTYPE_IPv4);
	else if (blockType == NETWORKTOOLS_PORTTYPE_IPv6) m_portBlockerType->setCurrentText(NETWORKTOOLS_PORTTYPE_IPv6);
}

bool NetworkTools::prepareToolShutdown(QSettings& _settings) {
	_settings.setValue("NET.BlockedPorts", m_portBlockerPorts->toPlainText());

	if (m_portBlockerType->currentText() == NETWORKTOOLS_PORTTYPE_IPv4) _settings.setValue("NET.BlockedPortsType", NETWORKTOOLS_PORTTYPE_IPv4);
	else if (m_portBlockerType->currentText() == NETWORKTOOLS_PORTTYPE_IPv6) _settings.setValue("NET.BlockedPortsType", NETWORKTOOLS_PORTTYPE_IPv6);
	else _settings.setValue("NET.BlockedPortsType", NETWORKTOOLS_PORTTYPE_ANY);

	return true;
}

// ###########################################################################################################################################################################################################################################################################################################################

void NetworkTools::slotRunPortBlocker(void) {
	if (m_status.flagIsSet(NetworkToolStatus::PortBlockerRunning)) {
		std::list<QTcpServer*> servers = m_servers;
		m_servers.clear();

		for (QTcpServer* server : servers) {
			server->close();
			delete server;
		}

		if (!servers.empty()) {
			OT_LOG_I("All running port blocker servers stopped");
		}

		m_portBlockerPorts->setReadOnly(false);
		m_portBlockerType->setEnabled(true);
		m_runPortBlockerButton->setText("Run");

		m_status.setFlag(NetworkToolStatus::PortBlockerRunning, false);
	}
	else {
		// Collect ports to block
		std::list<quint16> portsToBlock;
		std::list<std::string> portStrings = ot::splitString(m_portBlockerPorts->toPlainText().toStdString(), '\n', true);

		for (const std::string& port : portStrings) {
			size_t ix = port.find('-');
			
			if (ix == 0) {
				OT_LOG_E("Syntax error at port \"" + port + "\". Skipping...");
			}
			else if (ix == std::string::npos) {
				bool convertFailed = false;
				quint16 actualPort = ot::stringToNumber<quint16>(port, convertFailed);
				if (convertFailed) {
					OT_LOG_E("Invalid number format at port \"" + port + "\". Skipping...");
				}
				else {
					portsToBlock.push_back(actualPort);
				}
			}
			else {
				std::string fromPort = port.substr(0, ix);
				std::string toPort = port.substr(ix + 1);

				bool convertFromFailed = false;
				quint16 actualFromPort = ot::stringToNumber<quint16>(fromPort, convertFromFailed);
				
				bool convertToFailed = false;
				quint16 actualToPort = ot::stringToNumber<quint16>(toPort, convertToFailed);

				if (convertFromFailed || convertToFailed) {
					OT_LOG_E("Invalid number format at port range \"" + port + "\". Skipping...");
				}
				else {
					for (quint16 portNr = std::min(actualFromPort, actualToPort); portNr <= std::max(actualFromPort, actualToPort); portNr++) {
						portsToBlock.push_back(portNr);
					}
				}
			}
		}

		std::string successSuffix;
		QHostAddress::SpecialAddress listeningAdress = QHostAddress::Any;
		if (m_portBlockerType->currentText() == NETWORKTOOLS_PORTTYPE_IPv4) {
			listeningAdress = QHostAddress::AnyIPv4;
			successSuffix = ". Listening to IPv4";
		}
		else if (m_portBlockerType->currentText() == NETWORKTOOLS_PORTTYPE_IPv6) {
			listeningAdress = QHostAddress::AnyIPv6;
			successSuffix = ". Listening to IPv6";
		}

		// Start a tcp server for every port that needs to be blocked
		bool anyBlocked = false;
		for (quint16 port : portsToBlock) {
			QTcpServer* newServer = new QTcpServer;
			if (!newServer->listen(listeningAdress, port)) {
				delete newServer;
				newServer = nullptr;
				OT_LOG_E("Failed to start server on port (" + std::to_string(port) + ")");
			}
			else {
				OT_LOG_I("Port Blocker Server started on port (" + std::to_string(port) + ")" + successSuffix);
				m_servers.push_back(newServer);
				anyBlocked = true;
			}
		}

		// Ensure at least one port was blocked
		if (!anyBlocked) return;
		
		m_status.setFlag(NetworkToolStatus::PortBlockerRunning, true);
		m_portBlockerPorts->setReadOnly(true);
		m_portBlockerType->setEnabled(false);
		m_runPortBlockerButton->setText("Stop");
	}
}