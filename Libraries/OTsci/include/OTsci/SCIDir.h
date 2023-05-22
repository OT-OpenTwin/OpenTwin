//! @file SCIDir.h
//! @author Alexander Kuester (alexk95)
//! @date November 2022
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTsci/OTsciAPIExport.h"

#pragma warning(disable:4251)

// std header
#include <string>
#include <list>

namespace ot {
	class SCIFile;

	class OTSCI_API_EXPORT SCIDir {
	public:
		//! @brief Constructor.
		//! @param _name The name of the directory.
		//! @param _fullPath The full path to the directory.
		SCIDir(const std::wstring& _name, const std::wstring& _fullPath);
		SCIDir(const SCIDir& _other);
		virtual ~SCIDir();
		SCIDir& operator = (const SCIDir& _other);

		// ##########################################################

		// IO operations

		//! @brief Scan the contents of the current directory.
		//! Will use SCIOS
		void scanAll(void);

		//! @brief Scan all subdirectories.
		//! Will use SCIOS
		//! @param _scanSubdirectories If true, all subdirectories will be scanned, otherwise only top level.
		void scanDirectories(bool _scanSubdirectories = true);

		//! @brief Scan all files.
		//! Will use SCIOS
		//! @param _scanSubdirectories If true, all subdirectories will be scanned, otherwise only top level.
		void scanFiles(bool _scanSubdirectories = true);

		//! @brief Remove all directories from this object that don't contain any string from the provided whitelist.
		//! @param _list The whitelist.
		void filterDirectoriesWithWhitelist(const std::list<std::wstring>& _list);

		//! @brief Remove all directories from this object that contain any string from the provided blacklist.
		//! @param _list The blacklist.
		void filterDirectoriesWithBlacklist(const std::list<std::wstring>& _list);

		//! @brief Remove all files from this object that don't contain any string from the provided whitelist.
		//! @param _list The whitelist.
		void filterFilesWithWhitelist(const std::list<std::wstring>& _list);

		//! @brief Remove all files from this object that contain any string from the provided blacklist.
		//! @param _list The blacklist.
		void filterFilesWithBlacklist(const std::list<std::wstring>& _list);

		// ##########################################################

		// Setter

		//! @brief Remove all subdirectories and files from this object
		void clear(void);

		//! @brief Remove all subdirectories from this object.
		void clearSubdirectories(void);

		//! @brief Remove all files from this object.
		void clearFiles(void);

		// ##########################################################

		// Getter

		//! @brief Directory name.
		inline const std::wstring& name(void) const { return m_name; };

		//! @brief Full path to this directory.
		inline const std::wstring& fullPath(void) const { return m_fullPath; };

		//! @brief List of subdirectories found during the scan.
		inline const std::list<SCIDir>& subdirectories(void) const { return m_subDirectories; };

		//! @brief List containing directory names of the subdirectories found during the scan.
		std::list<std::wstring> subdirectoryNameList(void) const;

		//! @brief List of files found during the scan.
		inline const std::list<SCIFile>& files(void) const { return m_files; }

		//! @brief List containing file names of the files found during the scan.
		std::list<std::wstring> fileNameList(void) const;

		//! @brief Returns true if this directory exists at the given path
		bool exists(void) const;

		//! @brief Returns the number of subdirectories
		size_t subDirCount(void);

		//! @brief Returns the number of files
		size_t fileCount(void);

		// ##############################################################

		// Static functions

		//! @brief Returns the name only from the given path
		static std::wstring nameFromPath(const std::wstring& _path);

	private:
		std::wstring		m_name;
		std::wstring		m_fullPath;
		std::list<SCIDir>	m_subDirectories;
		std::list<SCIFile>	m_files;

		SCIDir() = delete;
	};
}