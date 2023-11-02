//! @file ToolMenuManager.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OToolkit header
#include "ToolMenuManager.h"

// Qt header
#include <QtWidgets/qaction.h>

ToolMenuManager::ToolMenuManager(const QString& _toolName)
	: QMenu(_toolName)
{
	m_run = this->addAction("Run");
	m_autorun = this->addAction("Autorun");
	m_autorun->setCheckable(true);

	this->connect(m_run, &QAction::triggered, this, &ToolMenuManager::slotRun);
	this->connect(m_autorun, &QAction::triggered, this, &ToolMenuManager::slotAutorun);
}

ToolMenuManager::~ToolMenuManager() {

}

QString ToolMenuManager::toolName(void) const {
	return this->title();
}

void ToolMenuManager::slotRun(void) {
	emit runRequested();
}

void ToolMenuManager::slotAutorun(void) {
	emit autorunChanged(m_autorun->isChecked());
}