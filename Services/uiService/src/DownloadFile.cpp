/*
 *	DownloadFile.cpp
 *
 *  Copyright (c) 2024 openTwin
 */

// OpenTwin header
#include "OTCore/Logger.h"
#include "DownloadFile.h"			// Corresponding header
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/Msg.h"

#include "base64.h"

#include "qprogressdialog.h"

#include <thread>
#include <chrono>
#include <cassert>
#include <filesystem>
#include <iostream>
#include <fstream>

#include <windows.h>

void downloadFrontendInstallerWorker(std::string _gssUrl, int _timeout, std::string *response, std::atomic<int> *success)
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_GetFrontendInstaller, doc.GetAllocator()), doc.GetAllocator());

	if (!ot::msg::send("", _gssUrl, ot::EXECUTE_ONE_WAY_TLS, doc.toJson(), *response, _timeout, ot::msg::DefaultFlagsNoExit))
	{
		*success = 0;
	}

	*success = 1;
}

bool downloadFrontendInstaller(
		const std::string&	_gssUrl,
		const std::string&	_fileName,
		std::string &		_tempFolder,
		std::string &		_error,
		QWidget*			_parent,
		int					_timeout)
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_PrepareFrontendInstaller, doc.GetAllocator()), doc.GetAllocator());
	std::string response;

	if (!ot::msg::send("", _gssUrl, ot::EXECUTE_ONE_WAY_TLS, doc.toJson(), response, _timeout, ot::msg::DefaultFlagsNoExit))
	{
		_error = "Unable to get frontend installer size.";
		OT_LOG_E(_error);
		return false;
	}
	
	size_t installerSize = 0;

	OT_ACTION_IF_RESPONSE_ERROR(response) 
	{
		installerSize = 140000000;
	}
	else
	{
		installerSize = atoll(response.c_str());
	}

	if (installerSize == 0)
	{
		// The frontend installer could not be found at the server side
		_error = "The frontend installer package could not be found on the server.";
		OT_LOG_E(_error);
		return false;
	}

	response.clear();
	response.reserve(installerSize);

	std::atomic<int> success = -1;

	OT_LOG("Downloading frontend installer from " + _gssUrl, ot::OUTGOING_MESSAGE_LOG);

	QProgressDialog progress("Download update package...", QString(), 0, 100);
	progress.setWindowTitle("Updating Frontend");
	progress.setMinimumWidth(300);
	progress.setWindowModality(Qt::WindowModal);
	progress.setValue(0);

	std::thread workerThread(downloadFrontendInstallerWorker, _gssUrl, _timeout, &response, &success);

	while (success == -1)
	{
		using namespace std::chrono_literals;
		std::this_thread::sleep_for(20ms);
		int percent = (int) (100.0 * response.size() / installerSize);
		progress.setValue(percent);
	}

	workerThread.join();

	if (success == 0)
	{
		_error = "Unable to download frontend installer.";
		OT_LOG_E(_error);
		return false;
	}

	_error.clear();
	char tempPath[MAX_PATH];

	// First create a temporary file name
	if (GetTempPathA(MAX_PATH, tempPath) == 0) {
		_error = "Unable to obtain temporary directory name.";
		OT_LOG_E(_error);
		return false;
	}

	char tempDirName[MAX_PATH];
	if (GetTempFileNameA(tempPath, "TMP", 0, tempDirName) == 0) {
		_error = "Unable to obtain temporary file name.";
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

