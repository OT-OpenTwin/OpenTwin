//! @file ToolMenuManager.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "AppBase.h"
#include "ToolMenuManager.h"

// OToolkitAPI header
#include "OToolkitAPI/OToolkitAPI.h"

// Qt header
#include <QtCore/qsettings.h>
#include <QtWidgets/qaction.h>

#define TOOLMENU_LOG(___msg) OTOOLKIT_LOG("ToolMenu", ___msg)
#define TOOLMENU_LOGW(___msg) OTOOLKIT_LOGW("ToolMenu", ___msg)
#define TOOLMENU_LOGE(___msg) OTOOLKIT_LOGE("ToolMenu", ___msg)

ToolMenuManager::ToolMenuManager(const QString& _toolName)
	: QMenu(_toolName), m_toolName(_toolName)
{
	m_run = this->addAction("Run");
	m_autorun = this->addAction("Autorun");
	this->addSeparator();
	m_autorun->setCheckable(true);

	AppBase* app = AppBase::instance();
	auto settings = app->createSettingsInstance();
	m_autorun->setChecked(settings->value("AutostartOption_" + m_toolName + "_", false).toBool());

	this->connect(m_run, &QAction::triggered, this, &ToolMenuManager::slotRun);
	this->connect(m_autorun, &QAction::triggered, this, &ToolMenuManager::slotAutorun);

	if (m_autorun->isChecked() ) {
		if (app->getIgnoreToolAutoStart()) {
			TOOLMENU_LOGW("Tool autorun ignored { \"ToolName\": " + _toolName + "\" }");
		}
		else {
			QMetaObject::invokeMethod(this, &ToolMenuManager::slotRun, Qt::QueuedConnection);
		}
	}
}

ToolMenuManager::~ToolMenuManager() {

}

void ToolMenuManager::slotRun(void) {
	m_run->setEnabled(false);
	Q_EMIT runRequested();
}

void ToolMenuManager::slotAutorun(bool _checked) {
	AppBase::instance()->createSettingsInstance()->setValue("AutostartOption_" + m_toolName + "_", _checked);
	Q_EMIT autorunChanged(_checked);
}
