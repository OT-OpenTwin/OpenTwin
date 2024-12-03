/*
 *	DownloadFile.cpp
 *
 *  Copyright (c) 2024 openTwin
 */

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCore/OTAssert.h"
#include "OTCommunication/DownloadFile.h"			// Corresponding header
#include "OTCommunication/ActionTypes.h"

// Curl header
#include "curl/curl.h"

#include <thread>
#include <chrono>
#include <cassert>
#include <filesystem>
#include <iostream>
#include <fstream>

size_t downloadFunction(void *ptr, size_t size, size_t nmemb, FILE* data) {
	fwrite(ptr, size, nmemb, data);
	return size * nmemb;
}

bool ot::DownloadFile::download(
		const std::string&	_fileUrl,
		const std::string&	_fileName,
		std::string &		_tempFolder,
		std::string &		_error,
		int					_timeout) 
{
	_error.clear();
	char tempPath[MAX_PATH];

	// First create a temporary file name
	if (GetTempPathA(MAX_PATH, tempPath) == 0) {
		OT_LOG_E("Unable to obtain temporary directory name");
		return false;
	}

	char tempDirName[MAX_PATH];
	if (GetTempFileNameA(tempPath, "TMP", 0, tempDirName) == 0) {
		OT_LOG_E("Unable to obtain temporary file name");
		return false;
	}

	if (!DeleteFileA(tempDirName)) {
		throw std::runtime_error("Unable to delete temporary file.");
	}

	if (!CreateDirectoryA(tempDirName, nullptr)) {
		throw std::runtime_error("Error when creating the temporary directory.");
	}

	_tempFolder = tempDirName;

	std::string fullPathName = _tempFolder + "\\" + _fileName;
	FILE* fp = fopen(fullPathName.c_str(), "wb");

	OT_LOG("Downloading file: " + _fileUrl + " to destination: " + fullPathName, ot::OUTGOING_MESSAGE_LOG);

	auto curl = curl_easy_init();
	if (!curl)
	{
		OTAssert(0, "Failed to initialize curl");
		OT_LOG_E("Failed to initialize curl");
		return false;
	}

	std::string _response;

	curl_easy_setopt(curl, CURLOPT_URL, _fileUrl.c_str());
	//curl_easy_setopt(curl, CURLOPT_POSTFIELDS, _message.c_str());
	curl_easy_setopt(curl, CURLOPT_USERAGENT, "libcrp/0.1");
	curl_easy_setopt(curl, CURLOPT_TIMEOUT, _timeout);

	std::string header_string;
	curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, downloadFunction);
	curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);

	char errbuf[CURL_ERROR_SIZE];
	curl_easy_setopt(curl, CURLOPT_ERRORBUFFER, errbuf);
	errbuf[0] = 0;

	// Send the request
	CURLcode errorCode = curl_easy_perform(curl);
	//std::cout << "URL Code received: " << res << std::endl;

	fclose(fp);

	curl_easy_cleanup(curl);

	if (errorCode == CURLE_OK)
	{
		OT_LOG(".. Downloading file completed successfully", ot::OUTGOING_MESSAGE_LOG);
		return true;
	}
	else
	{
		std::string errorString = curl_easy_strerror(errorCode);
		OT_LOG_D(
			".. Downloading file failed "
			"Error message: " + errorString + "; "
			"Error buffer: " + errbuf + "; "
			"(File = \"" + _fileUrl + "\"; "
			"Destination = " + fullPathName + "). );"
		);

		_error = "Downloading frontend installer failed. (" + errorString + ")";
		return false;
	}
}

