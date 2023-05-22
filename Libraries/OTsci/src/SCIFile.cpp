//! @file OTsciAPIExport.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2022
// ###########################################################################################################################################################################################################################################################################################################################

#include <aci/SCIFile.h>
#include <aci/OS.h>

using namespace ot;

SCIFile::SCIFile(const std::wstring& _filename, const std::wstring& _fullPath)
	: m_name(_filename), m_fullpath(_fullPath) {}

SCIFile::SCIFile(const SCIFile& _other) : m_name(_other.m_name), m_fullpath(_other.m_fullpath) {}

SCIFile::~SCIFile() {}

SCIFile& SCIFile::operator = (const SCIFile& _other) {
	m_name = _other.m_name;
	return *this;
}

// ##############################################################

bool SCIFile::exists(void) const {
	return OS::instance()->handler()->fileExists(m_fullpath);
}
