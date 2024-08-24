//! @file NetworkTools.h
//! @author Alexander Kuester (alexk95)
//! @date August 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// Toolkit API header
#include "OTCore/OTClassHelper.h"
#include "OToolkitAPI/Tool.h"

// Qt header
#include <QtCore/qobject.h>

// std header
#include <list>

class QTcpServer;
namespace ot { class ComboBox; }
namespace ot { class PushButton; }
namespace ot { class PlainTextEdit; }

class NetworkTools : public QObject, public otoolkit::Tool {
	Q_OBJECT
public:
	enum NetworkToolStatus {
		Idle               = 0x00,
		PortBlockerRunning = 0x01
	};
	typedef ot::Flags<NetworkToolStatus> NetworkToolStatusFlags;

	NetworkTools();
	virtual ~NetworkTools();

	// ###########################################################################################################################################################################################################################################################################################################################

	// API base functions

	//! @brief Return the unique tool name
	//! The name will be used to create all required menu entries
	virtual QString toolName(void) const override { return QString("Network Tools"); };

	//! @brief Create the central widget that will be displayed to the user in the main tab view
	virtual bool runTool(QMenu* _rootMenu, otoolkit::ToolWidgets& _content) override;

	virtual void restoreToolSettings(QSettings& _settings) override;

	//! @brief Stop all the logic of this tool
	virtual bool prepareToolShutdown(QSettings& _settings) override;

	// ###########################################################################################################################################################################################################################################################################################################################

private Q_SLOTS:
	void slotRunPortBlocker(void);
	
private:
	NetworkToolStatusFlags m_status;

	std::list<QTcpServer*> m_servers;
	ot::ComboBox* m_portBlockerType;
	ot::PushButton* m_runPortBlockerButton;
	ot::PlainTextEdit* m_portBlockerPorts;
	
};

OT_ADD_FLAG_FUNCTIONS(NetworkTools::NetworkToolStatus)