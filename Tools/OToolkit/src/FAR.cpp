//! @file FAR.cpp
//! @author Alexander Kuester (alexk95)
//! @date November 2023
// ###########################################################################################################################################################################################################################################################################################################################

// Toolkit header
#include "FAR.h"
#include "OToolkitAPI/OToolkitAPI.h"

// Qt header
#include <QtCore/qdir.h>
#include <QtCore/qfile.h>
#include <QtWidgets/qsplitter.h>

#define FAR_LOG(___message) OTOOLKIT_LOG("FAR", ___message)
#define FAR_LOGW(___message) OTOOLKIT_LOGW("FAR", ___message)
#define FAR_LOGE(___message) OTOOLKIT_LOGE("FAR", ___message)

FARFile::FARFile(const QString& _fullPath) {
	m_fullPath = _fullPath;
	m_fullPath.replace('\\', '/');
	m_nameOnly = m_fullPath.split('/').last();
	m_fileExtension = m_nameOnly.split('.').last();
}

FARFile::FARFile(const FARFile& _other) : m_fileExtension(_other.m_fileExtension), m_fullPath(_other.m_fullPath), m_nameOnly(_other.m_nameOnly) {}

void FARFile::findLineContains(const QString& _line) {

}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

FARDirectory::FARDirectory(const QString& _fullPath) {
	m_fullPath = _fullPath;
	m_fullPath.replace('\\', '/');
	m_nameOnly = m_fullPath.split('/').last();
}

FARDirectory::FARDirectory(const FARDirectory& _parentDirectory, const QString& _directoryName) {
	m_fullPath = _parentDirectory.fullPath() + '/' + _directoryName;
	m_nameOnly = _directoryName;
}

FARDirectory::FARDirectory(const FARDirectory& _other) : m_fullPath(_other.m_fullPath), m_nameOnly(_other.m_nameOnly) {

}

std::list<FARFile> FARDirectory::findFiles(const QStringList& _whitelist, const QStringList& _blacklist) {
	std::list<FARFile> ret;

	QDir directory(m_fullPath);
	if (!directory.exists()) {
		FAR_LOGE("Directory does not exist: \"" + m_fullPath + "\"");
	}
	else {
		QStringList files = directory.entryList(QDir::Files | QDir::NoDotAndDotDot);
		for (QString f : files) {
			FARFile newFile(this->fullPath() + '/' + f);
			if (this->isEntryOk(newFile.fullPath(), _whitelist, _blacklist)) {
				ret.push_back(newFile);
			}
		}
	}
	return ret;
}

std::list<FARDirectory> FARDirectory::findDirectories(const QStringList& _whitelist, const QStringList& _blacklist) {
	std::list<FARDirectory> ret;

	QDir directory(m_fullPath);
	if (!directory.exists()) {
		FAR_LOGE("Directory does not exist: \"" + m_fullPath + "\"");
	}
	else {
		QStringList subdirectories = directory.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
		for (QString d : subdirectories) {
			FARDirectory newDir(*this, d);
			if (this->isEntryOk(newDir.fullPath(), _whitelist, _blacklist)) {
				ret.push_back(newDir);
			}
		}
	}
	return ret;
}

bool FARDirectory::isEntryOk(const QString& _entry, const QStringList& _whitelist, const QStringList& _blacklist) const {
	// Whitelist
	if (!_whitelist.isEmpty()) {
		bool wok = false;
		for (const auto& e : _whitelist) {
			if (_entry.contains(e, Qt::CaseInsensitive)) {
				wok = true;
				break;
			}
		}
		if (!wok) return false;
	}

	// Blacklist
	for (const auto& e : _blacklist) {
		if (_entry.contains(e, Qt::CaseInsensitive)) {
			return false;
		}
	}
	return true;
}

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

// ###########################################################################################################################################################################################################################################################################################################################

FAR::FAR() {

}

FAR::~FAR() {

}

// ###########################################################################################################################################################################################################################################################################################################################

// API base functions

QString FAR::toolName(void) const {
	return "FAR";
}

QWidget* FAR::runTool(QMenu* _rootMenu, std::list<QWidget*>& _statusWidgets, QSettings& _settings) {
	m_centralSplitter = new QSplitter;


	return m_centralSplitter;
}

bool FAR::prepareToolShutdown(QSettings& _settings) {
	return true;
}
