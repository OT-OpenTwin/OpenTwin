// @otlicense

// OpenTwin header
#include "SDSServiceEntry.h"
#include "OToolkitAPI/OToolkitAPI.h"
#include "OTCore/ReturnMessage.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"

// Qt header
#include <QtWidgets/qlayout.h>

#define SDS_LOG(___message) OTOOLKIT_LOG("DebugSwitch", ___message)
#define SDS_LOGW(___message) OTOOLKIT_LOGW("DebugSwitch", ___message)
#define SDS_LOGE(___message) OTOOLKIT_LOGE("DebugSwitch", ___message)

SDSServiceEntry::SDSServiceEntry(ot::TreeWidget* _tree, const std::string& _lssUrl, const std::string& _serviceName, bool _isDebug) 
	: m_isDebug(_isDebug), m_lssUrl(_lssUrl), m_name(_serviceName)
{
	setText(static_cast<int>(ColumnIndex::ServiceName), QString::fromStdString(_serviceName));

	QWidget* sliderWrapper = new QWidget(_tree);
	QGridLayout* sliderLayout = new QGridLayout(sliderWrapper);

	m_toggleSlider = new ot::OnOffSlider(sliderWrapper);
	m_toggleSlider->setChecked(_isDebug);
	connect(m_toggleSlider, &ot::OnOffSlider::toggled, this, &SDSServiceEntry::debugToggled);
	sliderLayout->addWidget(m_toggleSlider, 0, 0, Qt::AlignCenter);
	sliderLayout->setContentsMargins(2, 2, 2, 2);

	setFlags(Qt::ItemIsEnabled | Qt::ItemNeverHasChildren);

	_tree->addTopLevelItem(this);
	_tree->setItemWidget(this, static_cast<int>(SDSServiceEntry::ColumnIndex::ToggleButton), sliderWrapper);
}

void SDSServiceEntry::toggle() {
	m_toggleSlider->toggle();
}

void SDSServiceEntry::debugToggled(bool _checked) {
	using namespace ot;

	if (_checked == m_isDebug) {
		return;
	}

	JsonDocument request;
	
	if (_checked) {
		request.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_ServiceEnableDebug, request.GetAllocator()), request.GetAllocator());
	}
	else {
		request.AddMember(OT_ACTION_MEMBER, JsonString(OT_ACTION_CMD_ServiceDisableDebug, request.GetAllocator()), request.GetAllocator());
	}

	request.AddMember(OT_ACTION_PARAM_SERVICE_NAME, JsonString(m_name, request.GetAllocator()), request.GetAllocator());

	std::string responseStr;
	if (!msg::send("", m_lssUrl, ot::EXECUTE, request.toJson(), responseStr, 0, msg::DefaultFlagsNoExit)) {
		SDS_LOGE(QString("Failed to send \"") + (_checked ? OT_ACTION_CMD_ServiceEnableDebug : OT_ACTION_CMD_ServiceDisableDebug) + "\" request for service \"" + QString::fromStdString(m_name) + "\" to LSS at \"" + QString::fromStdString(m_lssUrl) + "\"");
		m_toggleSlider->setChecked(m_isDebug);
		return;
	}

	ot::ReturnMessage returnMsg = ot::ReturnMessage::fromJson(responseStr);
	if (!returnMsg.isOk()) {
		SDS_LOGE(QString("Failed to ") + (_checked ? "enable" : "disable") + " debug for service \"" + QString::fromStdString(m_name) + "\" (LSS: " + QString::fromStdString(m_lssUrl) + "): " + QString::fromStdString(returnMsg.getWhat()));
		m_toggleSlider->setChecked(m_isDebug);
		return;
	}
	else {
		SDS_LOG(QString("Successfully ") + (_checked ? "enabled" : "disabled") + " debug for service \"" + QString::fromStdString(m_name) + "\" (LSS: " + QString::fromStdString(m_lssUrl) + ")");
		m_isDebug = _checked;
	}
}