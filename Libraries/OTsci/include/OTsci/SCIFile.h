//! @file OTsciAPIExport.h
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

#include "OTsci/OTsciAPIExport.h"

#pragma warning(disable:4251)

#include <string>

namespace aci {

	class OTSCI_API_EXPORT aFile {
	public:
		aFile(const std::wstring& _filename, const std::wstring& _fullPath);
		aFile(const aFile& _other);
		virtual ~aFile();
		aFile& operator = (const aFile& _other);

		// ##############################################################

		// Getter

		const std::wstring& name(void) const { return m_name; }
		const std::wstring& fullPath(void) const { return m_fullpath; }
		bool exists(void) const;

	private:
		std::wstring	m_name;
		std::wstring	m_fullpath;

		aFile() = delete;
	};
}