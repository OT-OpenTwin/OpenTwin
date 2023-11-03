//! @file FAR.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

// Toolkit header
#include "FAR.h"

// Qt header
#include <QtWidgets/qsplitter.h>

FAR::FAR() {

}

FAR::~FAR() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// API base functions

QString FAR::toolName(void) const {
	return "FAR";
}

QWidget* FAR::runTool(QMenu* _rootMenu, std::list<QWidget*>& _statusWidgets) {
	m_centralSplitter = new QSplitter;


	return m_centralSplitter;
}

bool FAR::prepareToolShutdown(void) {
	return true;
}
