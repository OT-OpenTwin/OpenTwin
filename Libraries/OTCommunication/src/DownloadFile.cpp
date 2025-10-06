/*
 *	DownloadFile.cpp
 *
 *  Copyright (c) 2024 openTwin
 */

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTCommunication/DownloadFile.h"			// Corresponding header
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/Msg.h"

#include "base64.h"

// Curl header
#include "curl/curl.h"

#include <thread>
#include <chrono>
#include <cassert>
#include <filesystem>
#include <iostream>
#include <fstream>

bool downloadFrontendInstaller(
		const std::string&	_gssUrl,
		const std::string&	_fileName,
		std::string &		_tempFolder,
		std::string &		_error,
		int					_timeout) 
{
	std::string response;
	response.reserve(200e6);

	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_GetFrontendInstaller, doc.GetAllocator()), doc.GetAllocator());

	OT_LOG("Downloading frontend installer from " + _gssUrl, ot::OUTGOING_MESSAGE_LOG);

	if (!ot::msg::send("", _gssUrl, ot::EXECUTE_ONE_WAY_TLS, doc.toJson(), response, _timeout, false, false))
	{
		_error = "Unable to download frontend installer";
		OT_LOG_E(_error);
		return false;
	}

	_error.clear();
	char tempPath[MAX_PATH];

	// First create a temporary file name
	if (GetTempPathA(MAX_PATH, tempPath) == 0) {
		_error = "Unable to obtain temporary directory name";
		OT_LOG_E(_error);
		return false;
	}

	char tempDirName[MAX_PATH];
	if (GetTempFileNameA(tempPath, "TMP", 0, tempDirName) == 0) {
		_error = "Unable to obtain temporary file name";
		OT_LOG_E(_error);
		return false;
	}

	if (!DeleteFileA(tempDirName)) {
		_error = "Unable to delete temporary file." + std::string(tempDirName);
		OT_LOG_E(_error);
		return false;
	}

	if (!CreateDirectoryA(tempDirName, nullptr)) {
		_error = "Error when creating the temporary directory: " + std::string(tempDirName);
		OT_LOG_E(_error);
		return false;
	}

	_tempFolder = tempDirName;

	// Decode result string
	int decoded_data_length = Base64decode_len(response.c_str());
	char* decodedContent = new char[decoded_data_length];

	Base64decode(decodedContent, response.c_str());

	// Save frontend installer
	std::string fullPathName = _tempFolder + "\\" + _fileName;

	std::ofstream file(fullPathName, std::ios::binary);
	file.write(decodedContent, decoded_data_length);
	file.close();

	delete[] decodedContent;
	decodedContent = nullptr;

	return true;
}

