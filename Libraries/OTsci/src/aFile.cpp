//! @file OTsciAPIExport.h
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

#include <aci/aFile.h>
#include <aci/OS.h>

using namespace aci;

aFile::aFile(const std::wstring& _filename, const std::wstring& _fullPath)
	: m_name(_filename), m_fullpath(_fullPath) {}

aFile::aFile(const aFile& _other) : m_name(_other.m_name), m_fullpath(_other.m_fullpath) {}

aFile::~aFile() {}

aFile& aFile::operator = (const aFile& _other) {
	m_name = _other.m_name;
	return *this;
}

// ##############################################################

bool aFile::exists(void) const {
	return OS::instance()->handler()->fileExists(m_fullpath);
}
