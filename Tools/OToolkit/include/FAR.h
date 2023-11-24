//! @file FAR.h
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// Toolkit API header
#include "OToolkitAPI/Tool.h"

class QSplitter;

class FAR : public otoolkit::Tool {
public:
	FAR();
	virtual ~FAR();

	// ###########################################################################################################################################################################################################################################################################################################################

	// API base functions

	//! @brief Return the unique tool name
	//! The name will be used to create all required menu entries
	virtual QString toolName(void) const override;

	//! @brief Create the central widget that will be displayed to the user in the main tab view
	virtual QWidget* runTool(QMenu* _rootMenu, std::list<QWidget*>& _statusWidgets, QSettings& _settings) override;

	//! @brief Stop all the logic of this tool
	virtual bool prepareToolShutdown(QSettings& _settings) override;

private:
	QSplitter* m_centralSplitter;

};