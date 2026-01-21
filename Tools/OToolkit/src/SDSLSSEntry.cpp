// @otlicense

// OpenTwin header
#include "SDSLSSEntry.h"
#include "SDSServiceEntry.h"
#include "OTWidgets/TreeWidget.h"
#include "OTWidgets/PushButton.h"
#include "OTWidgets/TreeWidgetFilter.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/LSSDebugInfo.h"
#include "OToolkitAPI/OToolkitAPI.h"

// Qt header
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qheaderview.h>

// std header
#include <set>
#include <thread>

#define SDS_LOG(___message) OTOOLKIT_LOG("DebugSwitch", ___message)
#define SDS_LOGW(___message) OTOOLKIT_LOGW("DebugSwitch", ___message)
#define SDS_LOGE(___message) OTOOLKIT_LOGE("DebugSwitch", ___message)

SDSLSSEntry::SDSLSSEntry(const ot::ServiceBase& _serviceInfo, QWidget* _parent)
	: m_serviceInfo(_serviceInfo), m_expander(nullptr), m_tree(nullptr)
{
	using namespace ot;

	QString title = QString::fromStdString(m_serviceInfo.getServiceName());
	m_expander = new ExpanderWidget(title, _parent);

	QWidget* contentWidget = new QWidget(m_expander);
	contentWidget->setMinimumHeight(600);
	m_expander->setWidget(contentWidget);

	QVBoxLayout* contentLayout = new QVBoxLayout(contentWidget);
	QHBoxLayout* headerLayout = new QHBoxLayout;
	contentLayout->addLayout(headerLayout);

	headerLayout->addStretch();
	PushButton* refreshButton = new PushButton("Refresh " + title, contentWidget);
	connect(refreshButton, &PushButton::clicked, this, &SDSLSSEntry::refreshData);
	headerLayout->addWidget(refreshButton);

	TreeWidgetFilter* tree = new TreeWidgetFilter(contentWidget);
	m_tree = tree->getTreeWidget();
	contentLayout->addWidget(tree->getQWidget(), 1);

	m_tree->setColumnCount(static_cast<int>(SDSServiceEntry::ColumnIndex::ColumnCount) + 1);
	QStringList headers;
	headers.append("Name");
	while (headers.size() < m_tree->columnCount()) {
		headers.append("");
	}
	m_tree->setHeaderLabels(headers);
	m_tree->setRootIsDecorated(false);

	m_tree->header()->setSectionResizeMode(QHeaderView::ResizeToContents);
	m_tree->header()->setSectionResizeMode(static_cast<int>(SDSServiceEntry::ColumnIndex::ColumnCount), QHeaderView::Stretch);
}

void SDSLSSEntry::refreshData() {
	m_tree->clear();
	m_tree->setEnabled(false);

	SDS_LOG("Fetching service information from LSS at \"" + QString::fromStdString(m_serviceInfo.getServiceURL()) + "\"");

	std::thread worker(&SDSLSSEntry::workerLoadData, this);
	worker.detach();
}

void SDSLSSEntry::workerFinished(const ServiceDebugInfoList& _services) {
	for (const ServiceDebugInfo& service : _services) {
		new SDSServiceEntry(m_tree, m_serviceInfo.getServiceURL(), service.serviceName, service.isDebug);
	}

	m_tree->sortByColumn(static_cast<int>(SDSServiceEntry::ColumnIndex::ServiceName), Qt::AscendingOrder);
	m_tree->setEnabled(true);
}

void SDSLSSEntry::itemDoubleClicked(QTreeWidgetItem* _item, int _column) {
	SDSServiceEntry* entry = dynamic_cast<SDSServiceEntry*>(_item);
	if (entry) {
		entry->toggle();
	}
	else {
		SDS_LOGW("Double clicked item is not of type SDSServiceEntry");
	}
}

void SDSLSSEntry::workerLoadData() {
	using namespace ot;

	ServiceDebugInfoList serviceList;
	
	JsonDocument requestDoc;
	requestDoc.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_GetDebugInformation, requestDoc.GetAllocator()), requestDoc.GetAllocator());

	std::string responseStr;
	if (!msg::send("", m_serviceInfo.getServiceURL(), ot::EXECUTE, requestDoc.toJson(), responseStr, 0, msg::DefaultFlagsNoExit)) {
		SDS_LOGE("Failed to send request to LSS at " + QString::fromStdString(m_serviceInfo.getServiceURL()));
		QMetaObject::invokeMethod(this, &SDSLSSEntry::workerFinished, Qt::QueuedConnection, serviceList);
		return;
	}

	JsonDocument responseDoc;
	if (!responseDoc.fromJson(responseStr)) {
		SDS_LOGE("Failed to parse response from LSS at " + QString::fromStdString(m_serviceInfo.getServiceURL()));
		QMetaObject::invokeMethod(this, &SDSLSSEntry::workerFinished, Qt::QueuedConnection, serviceList);
		return;
	}

	LSSDebugInfo debugInfo;
	debugInfo.setFromJsonObject(responseDoc.getConstObject());

	std::set<std::string> debugServices;
	for (const std::string& service : debugInfo.getDebugServices()) {
		debugServices.insert(service);
	}

	std::set<std::string> serviceNames;
	for (const auto& sessionType : debugInfo.getMandatoryServices()) {
		for (const auto& serviceName : sessionType.second) {
			serviceNames.insert(serviceName);
		}
	}

	for (const std::string& serviceName : serviceNames) {
		ServiceDebugInfo info;
		info.serviceName = serviceName;
		info.isDebug = (debugServices.find(serviceName) != debugServices.end());
		serviceList.push_back(std::move(info));
	}

	QMetaObject::invokeMethod(this, &SDSLSSEntry::workerFinished, Qt::QueuedConnection, serviceList);
}
