//! @file SCIDir.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2022
// ###########################################################################################################################################################################################################################################################################################################################

#include "OTsci/SCIDir.h"
#include "OTsci/SCIFile.h"
#include "OTsci/SCIOS.h"
#include "OpenTwinCore/StringHelper.h"

ot::SCIDir::SCIDir(const std::wstring& _name, const std::wstring& _fullPath) 
	: m_name(_name), m_fullPath(_fullPath) {}

ot::SCIDir::SCIDir(const SCIDir& _other)
	: m_name(_other.m_name), m_fullPath(_other.m_fullPath), m_subDirectories(_other.m_subDirectories), m_files(_other.m_files) {}

ot::SCIDir::~SCIDir() {}

ot::SCIDir& ot::SCIDir::operator = (const SCIDir& _other) {
	m_name = _other.m_name;
	m_fullPath = _other.m_fullPath;
	m_subDirectories = _other.m_subDirectories;
	m_files = _other.m_files;

	return *this;
}

// ##########################################################

// IO operations

void ot::SCIDir::scanAll(void) {
	clearSubdirectories();
	AbstractOSHandler * os = OS::instance()->handler();
	for (auto s : os->subdirectories(fullPath(), true)) {
		SCIDir * newSub = new SCIDir(s, fullPath() + L"/" + s);
		newSub->scanAll();
		m_subDirectories.push_back(newSub);
	}
	scanFiles(true);
}

void ot::SCIDir::scanDirectories(bool _scanSubdirectories) {
	clearSubdirectories();

	AbstractOSHandler * os = OS::instance()->handler();
	for (auto s : os->subdirectories(fullPath(), true)) {
		SCIDir * newSub = new SCIDir(s, fullPath() + L"/" + s);
		if (_scanSubdirectories && s != L"." && s != L"..") { newSub->scanDirectories(true); }
		m_subDirectories.push_back(newSub);
	}
}

void ot::SCIDir::scanFiles(bool _scanSubdirectories) {
	clearFiles();

	AbstractOSHandler * os = OS::instance()->handler();
	for (auto s : os->filesInDirectory(fullPath(), true)) {
		SCIFile * newFile = new SCIFile(s, fullPath() + L"/" + s);
		m_files.push_back(newFile);
	}

	if (_scanSubdirectories) {
		for (auto sub : m_subDirectories) {
			sub->scanFiles(_scanSubdirectories);
		}
	}
}

void ot::SCIDir::filterDirectoriesWithWhitelist(const std::list<std::wstring>& _list) {
	bool erased{ true };

	// Check subdirectories
	while (erased) {
		erased = false;
		for (auto it = m_subDirectories.begin(); it != m_subDirectories.end(); it++) {
			bool ok{ false };
			for (auto filter : _list) {
				if (it._Ptr->_Myval->name().find(filter) != std::wstring::npos || it._Ptr->_Myval->fullPath().find(filter) != std::wstring::npos) { ok = true; break; }
			}
			if (!ok) {
				m_subDirectories.erase(it);
				erased = true;
				break;
			}
		}
	}

	// Check subs of subs
	for (auto sub : m_subDirectories) {
		sub->filterDirectoriesWithWhitelist(_list);
	}
}

void ot::SCIDir::filterDirectoriesWithBlacklist(const std::list<std::wstring>& _list) {
	bool erased{ true };

	// Check subdirectories
	while (erased) {
		erased = false;
		for (auto it = m_subDirectories.begin(); it != m_subDirectories.end(); it++) {
			for (auto filter : _list) {
				if (it._Ptr->_Myval->name().find(filter) != std::wstring::npos || it._Ptr->_Myval->fullPath().find(filter) != std::wstring::npos) {
					m_subDirectories.erase(it);
					filterDirectoriesWithBlacklist(_list);
					return;
				}
			}

			if (erased) { break; }
		}
	}
	
	// Check subs of subs
	for (auto sub : m_subDirectories) {
		sub->filterDirectoriesWithBlacklist(_list);
	}
}

void ot::SCIDir::filterFilesWithWhitelist(const std::list<std::wstring>& _list) {
	bool erased{ true };

	// Check files
	erased = true;
	while (erased) {
		erased = false;
		for (auto it = m_files.begin(); it != m_files.end(); it++) {
			bool ok{ false };
			for (auto filter : _list) {
				if (it._Ptr->_Myval->name().find(filter) != std::wstring::npos || it._Ptr->_Myval->fullPath().find(filter) != std::wstring::npos) { ok = true; break; }
			}
			if (!ok) {
				m_files.erase(it);
				erased = true;
				break;
			}
		}
	}

	// Check subs of subs
	for (auto sub : m_subDirectories) {
		sub->filterFilesWithWhitelist(_list);
	}
}

void ot::SCIDir::filterFilesWithBlacklist(const std::list<std::wstring>& _list) {
	bool erased{ true };

	// Check files
	erased = true;
	while (erased) {
		erased = false;
		for (auto it = m_files.begin(); it != m_files.end(); it++) {
			for (auto filter : _list) {
				if (it._Ptr->_Myval->name().find(filter) != std::wstring::npos || it._Ptr->_Myval->fullPath().find(filter) != std::wstring::npos) {
					m_files.erase(it);
					filterFilesWithBlacklist(_list);
					return;
				}
			}

			if (erased) { break; }
		}
	}

	// Check subs of subs
	for (auto sub : m_subDirectories) {
		sub->filterFilesWithBlacklist(_list);
	}
}

// ##########################################################

// Setter

void ot::SCIDir::clear(void) {
	this->clearSubdirectories();
	this->clearFiles();
}

void ot::SCIDir::clearSubdirectories(void) {
	m_subDirectories.clear();
}

void ot::SCIDir::clearFiles(void) {
	m_files.clear();
}

// ##############################################################

// Getter

std::list<std::wstring> ot::SCIDir::subdirectoryNameList(void) const {
	std::list<std::wstring> ret;
	for (auto sub : m_subDirectories) {
		ret.push_back(sub->name());
		for (auto subsub : sub->subdirectoryNameList()) {
			ret.push_back(sub->name() + L"/" + subsub);
		}
	}
	return ret;
}

std::list<std::wstring> ot::SCIDir::fileNameList(void) const {
	std::list<std::wstring> ret;
	for (auto f : m_files) {
		ret.push_back(f->name());
	}
	for (auto sub : m_subDirectories) {
		for (auto subFile : sub->fileNameList()) {
			ret.push_back(sub->name() + L"/" + subFile);
		}
	}
	return ret;
}

bool ot::SCIDir::exists(void) const {
	AbstractOSHandler * os = OS::instance()->handler();
	return os->directoryExists(fullPath());
}

size_t ot::SCIDir::subDirCount(void) {
	size_t ret = m_subDirectories.size();
	for (auto sub : m_subDirectories) {
		ret += sub->subDirCount();
	}
	return ret;
}

size_t ot::SCIDir::fileCount(void) {
	size_t ret = m_files.size();
	for (auto sub : m_subDirectories) {
		ret += sub->fileCount();
	}
	return ret;
}

// ##############################################################

// Static functions

std::wstring ot::SCIDir::nameFromPath(const std::wstring& _path) {
	if (_path.empty()) { return _path; }
	std::wstring str = _path;
	for (size_t i{ 0 }; i < str.length(); i++) {
		if (str.at(i) == L'\\') { str.replace(i, 1, L"/"); }
	}
	std::list<std::wstring> lst = splitString(str, L"/");
	return lst.back();
}
