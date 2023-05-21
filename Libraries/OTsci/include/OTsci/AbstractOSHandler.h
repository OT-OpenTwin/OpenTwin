//! @file OTsciAPIExport.h
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTsci/OTsciAPIExport.h"

// std header
#include <string>
#include <list>

namespace aci {

	class InterpreterObject;

	class OTSCI_API_EXPORT AbstractOSHandler {
	public:
		AbstractOSHandler() {}
		virtual ~AbstractOSHandler() {}

		virtual bool fileExists(const std::wstring& _path) = 0;
		virtual bool deleteFile(const std::wstring& _path) = 0;
		virtual bool readFile(std::wstring& _data, const std::wstring& _path) = 0;
		virtual bool editFile(std::list<std::wstring>& _data, std::wstring& _path) = 0;
		virtual bool readLinesFromFile(std::list<std::wstring>& _data, const std::wstring& _path) = 0;
		virtual bool writeFile(const std::wstring& _data, const std::wstring& _path) = 0;
		virtual bool writeLinesToFile(const std::list<std::wstring>& _data, const std::wstring& _path) = 0;

		virtual bool directoryExists(const std::wstring& _path) = 0;
		virtual std::list<std::wstring> filesInDirectory(const std::wstring& _path, bool _searchTopLevelDirectoryOnly) = 0;
		virtual std::list<std::wstring> subdirectories(const std::wstring& _path, bool _searchTopLevelDirectoryOnly) = 0;
		virtual std::wstring currentDirectory(void) = 0;
		virtual std::wstring scriptDataDirectory(void) = 0;

		//! @brief Will request the next command from the Interpreter
		//! The request will be queued. When the user has finished the input the command will be send to the proivided object
		//! @param _obj The object the input will be send to
		virtual void requestNextCommand(InterpreterObject * _obj) = 0;

		virtual std::wstring getSettingsValue(const std::string& _key, const std::wstring& _defaultValue = std::wstring()) = 0;
		virtual void setSettingsValue(const std::string& _key, const std::wstring& _value) = 0;
	};
}