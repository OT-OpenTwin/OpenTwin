// @otlicense

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

class QLayout;
class QSplitter;
class QVBoxLayout;
namespace ot { class Table; }
namespace ot { class LineEdit; }
namespace ot { class PushButton; }
namespace ot { class ExpanderWidget; }
namespace ot { class IndicatorWidget; }

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
	enum class LDSServiceInfoMode {
		RequestedServices,
		InitializingServices,
		AliveServices,
		FailedServices,
		NewStoppingServices,
		StoppingServices
	};

	void loadWorker(std::string _gssUrl);

	bool gssLoad(const std::string& _gssUrl, ot::GSSDebugInfo& _gssInfo);
	bool lssLoad(const std::string& _lssUrl, ot::LSSDebugInfo& _lssInfo);
	bool gdsLoad(const std::string& _gdsUrl, ot::GDSDebugInfo& _gdsInfo);
	bool ldsLoad(const std::string& _ldsUrl, ot::LDSDebugInfo& _ldsInfo);

	// ###########################################################################################################################################################################################################################################################################################################################

	// Private: Widget Helper

	ot::Table* createGssSessionTable(const std::list<ot::GSSDebugInfo::SessionData>& _sessionData);
	ot::Table* createLssServicesTable(const std::list<ot::LSSDebugInfo::ServiceInfo>& _services);
	ot::Table* createGdsServicesTable(const std::list<ot::GDSDebugInfo::ServiceInfo>& _services);
	int createLDSServiceInfo(const ot::LDSDebugInfo::SessionInfo& _sessionInfo, const std::list<ot::LDSDebugInfo::ServiceInfo>& _info, LDSServiceInfoMode _mode, QLayout* _layout);

	ot::LineEdit* lineEdit(uint16_t _value);
	ot::LineEdit* lineEdit(const std::string& _text);
	ot::LineEdit* lineEdit(const QString& _text);
	ot::IndicatorWidget* checkBox(bool _checked);

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

	QWidget* m_stretchWidget;
};