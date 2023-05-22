//! @file OTsciAPIExport.h
//! @author Alexander Kuester (alexk95)
//! @date November 2022
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

#include "OTsci/OTsciAPIExport.h"

#pragma warning(disable:4251)

#include <string>

namespace ot {

	class OTSCI_API_EXPORT SCIFile {
	public:
		SCIFile(const std::wstring& _filename, const std::wstring& _fullPath);
		SCIFile(const SCIFile& _other);
		virtual ~SCIFile();
		SCIFile& operator = (const SCIFile& _other);

		// ##############################################################

		// Getter

		const std::wstring& name(void) const { return m_name; }
		const std::wstring& fullPath(void) const { return m_fullpath; }
		bool exists(void) const;

	private:
		std::wstring	m_name;
		std::wstring	m_fullpath;

		SCIFile() = delete;
	};
}