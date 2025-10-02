//! @file BackendInfo.h
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// API header
#include "OToolkitAPI/Tool.h"

// OpenTwin header
#include "OTCommunication/GSSDebugInfo.h"
#include "OTCommunication/LSSDebugInfo.h"
#include "OTCommunication/GDSDebugInfo.h"
#include "OTCommunication/LDSDebugInfo.h"

// Qt header
#include <QtCore/qobject.h>

// std header
#include <list>
#include <thread>

class QVBoxLayout;
namespace ot { class Table; }
namespace ot { class LineEdit; }
namespace ot { class PushButton; }
namespace ot { class ExpanderWidget; }

class BackendInfo : public QObject, public otoolkit::Tool {
	Q_OBJECT
public:
	BackendInfo();
	virtual ~BackendInfo();

	// ###########################################################################################################################################################################################################################################################################################################################

	// API base functions

	//! @brief Return the unique tool name
	//! The name will be used to create all required menu entries
	virtual QString getToolName(void) const override;

	//! @brief Create the central widget that will be displayed to the user in the main tab view
	virtual bool runTool(QMenu* _rootMenu, otoolkit::ToolWidgets& _content) override;

	virtual void restoreToolSettings(QSettings& _settings) override;

	//! @brief Stop all the logic of this tool
	virtual bool prepareToolShutdown(QSettings& _settings) override;

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Slots

private Q_SLOTS:
	void slotClear();
	void slotLoad();
	void slotCancel();
	void slotAddGSS(const ot::GSSDebugInfo& _info);
	void slotAddLSS(const ot::LSSDebugInfo& _info);
	void slotAddGDS(const ot::GDSDebugInfo& _info);
	void slotAddLDS(const ot::LDSDebugInfo& _info);
	void loadWorkerFinished();

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Worker

private:
	void loadWorker(std::string _gssUrl);

	bool gssLoad(const std::string& _gssUrl);

	ot::Table* createGssSessionTable(const std::list<ot::GSSDebugInfo::SessionData>& _sessionData);

	std::list<ot::GSSDebugInfo> m_gssInfos;
	std::list<ot::LSSDebugInfo> m_lssInfos;
	std::list<ot::GDSDebugInfo> m_gdsInfos;
	std::list<ot::LDSDebugInfo> m_ldsInfos;

	std::list<ot::ExpanderWidget*> m_sections;
	QVBoxLayout* m_sectionsLayout;

	ot::LineEdit* m_gssUrl;

	std::thread* m_loadThread;

	ot::PushButton* m_loadButton;
	ot::PushButton* m_cancelButton;
	ot::PushButton* m_clearButton;
};