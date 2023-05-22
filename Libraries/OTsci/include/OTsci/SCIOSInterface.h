//! @file SCIOSInterface.h
//! @author Alexander Kuester (alexk95)
//! @date November 2022
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTsci/OTsciAPIExport.h"

// std header
#include <string>
#include <list>

namespace ot {

	class InterpreterObject;

	class OTSCI_API_EXPORT SCIOSInterface {
	public:
		SCIOSInterface() {};
		virtual ~SCIOSInterface() {};

		// ###########################################################################################################################################################################################################################################################################################################################

		// File handling

		//! @brief Returns true if the file at the specified path exists
		//! @param _path Path to file
		virtual bool fileExists(const std::wstring& _path) = 0;

		//! @brief Delete the file at the specified path
		//! @param _path Path to file
		//! @return True if success
		virtual bool deleteFile(const std::wstring& _path) = 0;

		//! @brief Read the whole content of the specified file
		//! @param _path Path to file
		//! @param _data Data will be written here (assumed empty, string will not be cleared)
		//! @return True if success
		virtual bool readFromFile(const std::wstring& _path, std::string& _data) = 0;

		//! @brief Read the whole content of the specified file
		//! @param _path Path to file
		//! @param _data Data will be written here (assumed empty, string will not be cleared)
		//! @return True if success
		virtual bool readFromFile(const std::wstring& _path, std::wstring& _data) = 0;

		//! @brief Read line by line from the specified file
		//! @param _path Path to file
		//! @param _data Lines in the file
		//! @return True if success
		virtual bool readLinesFromFile(const std::wstring& _path, std::list<std::string>& _data) = 0;

		//! @brief Read line by line from the specified file
		//! @param _path Path to file
		//! @param _data Lines in the file
		//! @return True if success
		virtual bool readLinesFromFile(const std::wstring& _path, std::list<std::wstring>& _data) = 0;
		
		//! @brief Write the provided data to the specified file
		//! @param _data Data to write
		//! @param _path Path to file
		//! @param _append If true, the data should be appended to the file, otherwise overwrite file
		virtual bool writeToFile(const std::string& _data, const std::wstring& _path, bool _append = false) = 0;

		//! @brief Write the provided data to the specified file
		//! @param _data Data to write
		//! @param _path Path to file
		//! @param _append If true, the data should be appended to the file, otherwise overwrite file
		virtual bool writeToFile(const std::wstring& _data, const std::wstring& _path, bool _append = false) = 0;
		
		//! @brief Write the provided data line by line to the specified file
		//! @param _data Lines to write
		//! @param _path Path to file
		//! @param _append If true, the data should be appended to the file, otherwise overwrite file
		virtual bool writeLinesToFile(const std::list<std::string>& _data, const std::wstring& _path, bool _append = false) = 0;

		//! @brief Write the provided data line by line to the specified file
		//! @param _data Lines to write
		//! @param _path Path to file
		//! @param _append If true, the data should be appended to the file, otherwise overwrite file
		virtual bool writeLinesToFile(const std::list<std::wstring>& _data, const std::wstring& _path, bool _append = false) = 0;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Directory handling

		//! @brief Return true if the specified diretory exists
		virtual bool directoryExists(const std::wstring& _path) = 0;

		//! @brief Returns the full path to all files in the specified directory
		//! @param _path The path to search at
		//! @param _searchToLevelDirectoryOnly If false, subdirectories will be searched aswell
		virtual std::list<std::wstring> filesInDirectory(const std::wstring& _path, bool _searchTopLevelDirectoryOnly = true) = 0;

		//! @brief Returns the full path to all subdirectories in the specified directory
		//! @param _path The path to search at
		//! @param _searchToLevelDirectoryOnly If false, subdirectories will be searched aswell
		virtual std::list<std::wstring> subdirectories(const std::wstring& _path, bool _searchTopLevelDirectoryOnly = true) = 0;

		// ###########################################################################################################################################################################################################################################################################################################################

		// SCI data handling

		//! @brief Return the working directory
		virtual std::wstring currentWorkingDirectory(void) = 0;

		//! @brief Return the root directory to the data path
		//! This path will be used by the SCIObjects to store their data at
		virtual std::wstring objectDataRootDirectory(void) = 0;
	};
}