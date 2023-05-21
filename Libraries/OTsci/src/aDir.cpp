//! @file OTsciAPIExport.h
//! @author Alexander Kuester (alexk95)
//! @date March 2023
// ###########################################################################################################################################################################################################################################################################################################################

#include <aci/aDir.h>
#include <aci/aFile.h>
#include <aci/OS.h>
#include <aci/Convert.h>

using namespace aci;

aDir::aDir(const std::wstring& _name, const std::wstring& _fullPath) 
	: m_name(_name), m_fullPath(_fullPath) {}

aDir::aDir(const aDir& _other) 
	: m_name(_other.m_name), m_fullPath(_other.m_fullPath)
{
	for (auto sd : _other.m_subDirectories) {
		m_subDirectories.push_back(sd);
	}
	for (auto f : _other.m_files) {
		m_files.push_back(f);
	}
}

aDir::~aDir() {
	clearSubdirectories();
	clearFiles();
}

aDir& aDir::operator = (const aDir& _other) {
	m_name = _other.m_name;
	m_fullPath = _other.m_fullPath;

	clearSubdirectories();
	for (auto sd : _other.m_subDirectories) {
		m_subDirectories.push_back(sd);
	}
	clearFiles();
	for (auto f : _other.m_files) {
		m_files.push_back(f);
	}

	return *this;
}

// ##########################################################

// IO operations

void aDir::scanAll(void) {
	clearSubdirectories();
	AbstractOSHandler * os = OS::instance()->handler();
	for (auto s : os->subdirectories(fullPath(), true)) {
		aDir * newSub = new aDir(s, fullPath() + L"/" + s);
		newSub->scanAll();
		m_subDirectories.push_back(newSub);
	}
	scanFiles(true);
}

void aDir::scanDirectories(bool _scanSubdirectories) {
	clearSubdirectories();

	AbstractOSHandler * os = OS::instance()->handler();
	for (auto s : os->subdirectories(fullPath(), true)) {
		aDir * newSub = new aDir(s, fullPath() + L"/" + s);
		if (_scanSubdirectories && s != L"." && s != L"..") { newSub->scanDirectories(true); }
		m_subDirectories.push_back(newSub);
	}
}

void aDir::scanFiles(bool _scanSubdirectories) {
	clearFiles();

	AbstractOSHandler * os = OS::instance()->handler();
	for (auto s : os->filesInDirectory(fullPath(), true)) {
		aFile * newFile = new aFile(s, fullPath() + L"/" + s);
		m_files.push_back(newFile);
	}

	if (_scanSubdirectories) {
		for (auto sub : m_subDirectories) {
			sub->scanFiles(_scanSubdirectories);
		}
	}
}

void aDir::filterDirectoriesWithWhitelist(const std::list<std::wstring>& _list) {
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

void aDir::filterDirectoriesWithBlacklist(const std::list<std::wstring>& _list) {
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

void aDir::filterFilesWithWhitelist(const std::list<std::wstring>& _list) {
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

void aDir::filterFilesWithBlacklist(const std::list<std::wstring>& _list) {
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

void aDir::clearSubdirectories(void) {
	for (auto d : m_subDirectories) {
		delete d;
	}
	m_subDirectories.clear();
}

void aDir::clearFiles(void) {
	for (auto f : m_files) {
		delete f;
	}
	m_files.clear();
}

// ##############################################################

// Getter

std::list<std::wstring> aDir::subdirectoryNameList(void) const {
	std::list<std::wstring> ret;
	for (auto sub : m_subDirectories) {
		ret.push_back(sub->name());
		for (auto subsub : sub->subdirectoryNameList()) {
			ret.push_back(sub->name() + L"/" + subsub);
		}
	}
	return ret;
}

std::list<std::wstring> aDir::fileNameList(void) const {
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

bool aDir::exists(void) const {
	AbstractOSHandler * os = OS::instance()->handler();
	return os->directoryExists(fullPath());
}

size_t aDir::subDirCount(void) {
	size_t ret = m_subDirectories.size();
	for (auto sub : m_subDirectories) {
		ret += sub->subDirCount();
	}
	return ret;
}

size_t aDir::fileCount(void) {
	size_t ret = m_files.size();
	for (auto sub : m_subDirectories) {
		ret += sub->fileCount();
	}
	return ret;
}

// ##############################################################

// Static functions

std::wstring aDir::lastFolder(const std::wstring& _path) {
	if (_path.empty()) { return _path; }
	std::wstring str = _path;
	for (size_t i{ 0 }; i < str.length(); i++) {
		if (str.at(i) == L'\\') { str.replace(i, 1, L"/"); }
	}
	std::list<std::wstring> lst = splitString(str, L"/");
	return lst.back();
}
