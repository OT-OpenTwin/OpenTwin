//! @file OTsciAPIExport.h
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

#include "OTsci/OTsciAPIExport.h"

#pragma warning(disable:4251)

#include <string>
#include <list>

namespace aci {
	class aFile;

	class OTSCI_API_EXPORT aDir {
	public:

		aDir(const std::wstring& _name, const std::wstring& _fullPath);
		aDir(const aDir& _other);
		virtual ~aDir();
		aDir& operator = (const aDir& _other);

		// ##########################################################

		// IO operations

		void scanAll(void);
		void scanDirectories(bool _scanSubdirectories = true);
		void scanFiles(bool _scanSubdirectories = true);
		void filterDirectoriesWithWhitelist(const std::list<std::wstring>& _list);
		void filterDirectoriesWithBlacklist(const std::list<std::wstring>& _list);
		void filterFilesWithWhitelist(const std::list<std::wstring>& _list);
		void filterFilesWithBlacklist(const std::list<std::wstring>& _list);

		// ##########################################################

		// Setter
		void clearSubdirectories(void);
		void clearFiles(void);

		// ##########################################################

		// Getter

		inline const std::wstring& name(void) const { return m_name; }
		inline const std::wstring& fullPath(void) const { return m_fullPath; }
		inline const std::list<aDir *>& subdirectories(void) const { return m_subDirectories; }
		std::list<std::wstring> subdirectoryNameList(void) const;
		inline const std::list<aFile *>& files(void) const { return m_files; }
		std::list<std::wstring> fileNameList(void) const;
		bool exists(void) const;
		size_t subDirCount(void);
		size_t fileCount(void);

		// ##############################################################

		// Static functions

		static std::wstring lastFolder(const std::wstring& _path);

	private:
		std::wstring		m_name;
		std::wstring		m_fullPath;
		std::list<aDir *>	m_subDirectories;
		std::list<aFile *>	m_files;

		aDir() = delete;
	};
}