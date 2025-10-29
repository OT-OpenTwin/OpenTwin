// @otlicense

/*
 *	DownloadFile.h
 *
 *  Copyright (c) 2024 openTwin
 */

#pragma once

#include "OTCommunication/CommunicationAPIExport.h"
#include "OTCommunication/CommunicationTypes.h"

 // C++ header
#include <string>
#include <qwidget.h>

bool downloadFrontendInstaller(
	const std::string &				_gssUrl,
	const std::string &				_fileName,
	std::string&					_tempFolder,
	std::string&					_error,
	QWidget*						_parent,
	int								_timeout = 5000
);
