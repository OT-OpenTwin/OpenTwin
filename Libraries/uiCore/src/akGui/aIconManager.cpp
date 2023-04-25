/*
 *	File:		aIconManager.cpp
 *	Package:	akGui
 *
 *  Created on: March 20, 2020
 *	Author: Alexander Kuester
 *  Copyright (c) 2022 Alexander Kuester
 *	This file is part of the uiCore component.
 *	This file is subject to the terms and conditions defined in
 *	file 'LICENSE', which is part of this source code package.
 */

// AK header
#include <akGui/aIconManager.h>
#include <akCore/aException.h>

// Qt header
#include <qicon.h>					// QIcon
#include <qdir.h>					// Check directories
#include <qfile.h>					// Check file
#include <qpixmap.h>
#include <qmovie.h>

// C++ header
#include <mutex>					// Thread protection

#define PATH_PIXMAPS "Images/"
#define PATH_MOVIES "Animations/"

ak::aIconManager::aIconManager(
	const QString &					_mainDirectory
) : m_fileExtension(".png"),
	m_mutex(nullptr)
{
	m_mutex = new std::mutex();
	// Check if a main directory was provided
	if (_mainDirectory.length() > 0) {
		QDir dir(_mainDirectory);
		if (!dir.exists()) {
			throw aException("Directory does not exist!", "ak::aIconManager::addDirectory()");
		}
		// Check for the directory delimiter
		QString directory = _mainDirectory;
		if (!directory.endsWith('/') && !directory.endsWith('\\')) { directory.append('/'); }
		// Add directory
		m_directories.push_back(directory);
	}
}

ak::aIconManager::~aIconManager() {
	if (m_mutex != nullptr) { 
		m_mutex->lock(); m_mutex->unlock();  delete m_mutex;  m_mutex = nullptr;
	}
	// Delete all imported icons
	for (auto icon = m_icons.begin(); icon != m_icons.end(); icon++) {
		if (icon->second != nullptr) {
			for (auto size = icon->second->begin(); size != icon->second->end(); size++) {
				if (size->second != nullptr) { delete size->second; size->second = nullptr; }
			}
			delete icon->second; icon->second = nullptr;
		}
	}

	for (auto pix = m_pixmaps.begin(); pix != m_pixmaps.end(); pix++) {
		delete pix->second;
	}
}

void ak::aIconManager::addDirectory(
	const QString &					_directory
) {
	// Lock the mutex
	m_mutex->lock();

	// Check if the specified directory does exist
	QDir dir(_directory);
	if (!dir.exists()) {
		m_mutex->unlock();
		throw aException("Provided directory does not exist!", "ak::aIconManager::addDirectory()");
	}
	// Check for the directory ending
	QString directory = _directory;
	directory.replace('\\', '/');
	if (!directory.endsWith('/') && !directory.endsWith('\\')) { directory.append('/'); }

	// Check if directory already exists
	for (int i = 0; i < m_directories.size(); i++) {
		if (m_directories.at(i).toLower() == directory.toLower()) { m_mutex->unlock(); return; }
	}
	m_directories.push_back(directory);
	m_mutex->unlock();
}

bool ak::aIconManager::removeDirectory(
	const QString &					_directory
) {
	// Lock the mutex
	m_mutex->lock();

	// Check for the directory ending to be able to check the
	QString directory = _directory;
	directory.replace('\\', '/');
	if (!directory.endsWith('/') && !directory.endsWith('\\')) { directory.append('/'); }
	for (int i = 0; i < m_directories.size(); i++) {
		// Check if the names are equal
		if (m_directories.at(i).toLower() == directory.toLower()) {
			m_directories.erase(m_directories.begin() + i);
			m_mutex->unlock();
			return true;
		}
	}
	m_mutex->unlock();
	assert(0); // The provided directory does not exist
	return false;
}

const QIcon * ak::aIconManager::icon(
	const QString &					_iconName,
	const QString &					_iconSize
) {
	try {
		m_mutex->lock();
		auto ico = m_icons.find(_iconName);
		if (ico == m_icons.end()) {
			// icon does not exist at all
			QIcon * newIcon = createIcon(_iconName, _iconSize);
			std::map<QString, QIcon *> * newMap = nullptr;
			newMap = new std::map<QString, QIcon *>;
			newMap->insert_or_assign(_iconSize, newIcon);
			m_icons.insert_or_assign(_iconName, newMap);
			m_mutex->unlock();
			return newIcon;
		}
		else {
			auto size = ico->second->find(_iconSize);
			if (size == ico->second->end()) {
				// icon in this size does not exist
				QIcon * newIcon = createIcon(_iconName, _iconSize);
				ico->second->insert_or_assign(_iconSize, newIcon);
				m_mutex->unlock();
				return newIcon;
			}
			else {
				// icon does exist
				m_mutex->unlock();
				return size->second;
			}
		}
	}
	catch (const aException & e) { m_mutex->unlock(); throw aException(e, "ak::aIconManager::icon()"); }
	catch (const std::exception & e) { m_mutex->unlock(); throw aException(e.what(), "ak::aIconManager::icon()"); }
	catch (...) { m_mutex->unlock(); throw aException("Unknown error", "ak::aIconManager::icon()"); }
}

const QIcon * ak::aIconManager::applicationIcon(
	const QString &					_iconName
) {
	try {
		m_mutex->lock();
		auto ico = m_icons.find(_iconName);
		if (ico == m_icons.end()) {
			// icon does not exist at all
			QIcon * newIcon = createApplicationIcon(_iconName);
			std::map<QString, QIcon *> * newMap = nullptr;
			newMap = new std::map<QString, QIcon *>;
			newMap->insert_or_assign("Application", newIcon);
			m_icons.insert_or_assign(_iconName, newMap);
			m_mutex->unlock();
			return newIcon;
		}
		else {
			// icon does exist
			auto size = ico->second->find("Application");
			m_mutex->unlock();

			if (size == ico->second->end())
			{
				assert(0);
				return nullptr;
			}

			return size->second;
		}
	}
	catch (const aException & e) { m_mutex->unlock(); throw aException(e, "ak::aIconManager::applicationIcon()"); }
	catch (const std::exception & e) { m_mutex->unlock(); throw aException(e.what(), "ak::aIconManager::applicationIcon()"); }
	catch (...) { m_mutex->unlock(); throw aException("Unknown error", "ak::aIconManager::applicationIcon()"); }
}

const QPixmap * ak::aIconManager::pixmap(
	const QString &									_imageName
) {
	try {
		m_mutex->lock();
		auto itm = m_pixmaps.find(_imageName);
		if (itm != m_pixmaps.end()) { 
			m_mutex->unlock();
			return itm->second; 
		}
		QPixmap * pix = createPixmap(_imageName);
		m_pixmaps.insert_or_assign(_imageName, pix);
		m_mutex->unlock();
		return pix;
	}
	catch (const aException & e) { m_mutex->unlock(); throw aException(e, "ak::aIconManager::pixmap()"); }
	catch (const std::exception & e) { m_mutex->unlock(); throw aException(e.what(), "ak::aIconManager::pixmap()"); }
	catch (...) { m_mutex->unlock(); throw aException("Unknown error", "ak::aIconManager::pixmap()"); }
}

QMovie * ak::aIconManager::movie(
	const QString &									_imageName
) {
	try {
		m_mutex->lock();
		auto itm = m_movies.find(_imageName);
		if (itm != m_movies.end()) {
			m_mutex->unlock();
			return itm->second; 
		}
		QMovie * mv = createMovie(_imageName);
		m_movies.insert_or_assign(_imageName, mv);
		m_mutex->unlock();
		return mv;
	}
	catch (const aException & e) { m_mutex->unlock(); throw aException(e, "ak::aIconManager::movie()"); }
	catch (const std::exception & e) { m_mutex->unlock(); throw aException(e.what(), "ak::aIconManager::movie()"); }
	catch (...) { m_mutex->unlock(); throw aException("Unknown error", "ak::aIconManager::movie()"); }
}

void ak::aIconManager::setFileExtension(
	const QString &									_extension
) { 
	m_mutex->lock();
	m_fileExtension = _extension;
	m_mutex->unlock();
}

QString ak::aIconManager::getFileExtension() { 
	m_mutex->lock();
	QString ret = m_fileExtension;
	m_mutex->unlock();
	return ret;
}


QIcon * ak::aIconManager::createIcon(
	const QString &									_iconName,
	const QString &									_iconSubdirectory
) {
	// Mutex not required, caller must take care of the mutex
	try {
		for (int i = 0; i < m_directories.size(); i++) {
			QFile file(m_directories.at(i) + _iconSubdirectory + '/' + _iconName + m_fileExtension);
			// Check if the file exist
			if (file.exists()) {
				QIcon * ico = nullptr;
				ico = new QIcon(file.fileName());
				return ico;
			}
		}
		throw aException(("Icon \"" + _iconName + ".png\" in the subdirectory \"" + _iconSubdirectory + "\" does not exist").toStdString(), "Check status");
	}
	catch (const aException & e) { throw aException(e, "ak::aIconManager::createIcon()"); }
	catch (const std::exception & e) { throw aException(e.what(), "ak::aIconManager::createIcon()"); }
	catch (...) { throw aException("Unknown error", "ak::aIconManager::createIcon()"); }
}

QIcon * ak::aIconManager::createApplicationIcon(
	const QString &									_iconName
) {
	// Mutex not required, caller must take care of the mutex
	try {
		for (int i = 0; i < m_directories.size(); i++) {
			QFile file(m_directories.at(i) + "Application" + '/' + _iconName + ".ico");
			// Check if the file exist
			if (file.exists()) {
				QIcon * ico = nullptr;
				ico = new QIcon(file.fileName());
				return ico;
			}
		}
		throw aException(("Icon \"" + _iconName + ".ico\" in the subdirectory \"Application\" does not exist").toStdString(), "Check status");
	}
	catch (const aException & e) { throw aException(e, "ak::aIconManager::createApplicationIcon()"); }
	catch (const std::exception & e) { throw aException(e.what(), "ak::aIconManager::createApplicationIcon()"); }
	catch (...) { throw aException("Unknown error", "ak::aIconManager::createApplicationIcon()"); }
}

QPixmap * ak::aIconManager::createPixmap(
	const QString &									_imageName
) {
	// Mutex not required, caller must take care of the mutex
	try {
		for (int i = 0; i < m_directories.size(); i++) {
			QFile file(m_directories.at(i) + PATH_PIXMAPS + _imageName + m_fileExtension);
			// Check if the file exist
			if (file.exists()) {
				QPixmap * ico = nullptr;
				ico = new (std::nothrow) QPixmap(file.fileName());
				if (ico == nullptr) { throw aException("Failed to create", "Create icon"); }
				return ico;
			}
		}
		throw aException(("Image \"" + _imageName + "\" in the subdirectory \"" PATH_PIXMAPS "\" does not exist").toStdString(), "Check status");
	}
	catch (const aException & e) { throw aException(e, "ak::aIconManager::createPixmap()"); }
	catch (const std::exception & e) { throw aException(e.what(), "ak::aIconManager::createPixmap()"); }
	catch (...) { throw aException("Unknown error", "ak::aIconManager::createPixmap()"); }
}

QMovie * ak::aIconManager::createMovie(
	const QString &									_imageName
) {
	// Mutex not required, caller must take care of the mutex
	try {
		for (int i = 0; i < m_directories.size(); i++) {
			QFile file(m_directories.at(i) + PATH_MOVIES + _imageName + ".gif");
			// Check if the file exist
			if (file.exists()) {
				QMovie * mv = nullptr;
				mv = new (std::nothrow) QMovie(file.fileName());
				if (mv == nullptr) { throw aException("Failed to create", "Create movie"); }
				return mv;
			}
		}
		throw aException(("Movie \"" + _imageName + "\" in the subdirectory \"" PATH_MOVIES "\" does not exist").toStdString(), "Check status");
	}
	catch (const aException & e) { throw aException(e, "ak::aIconManager::createMovie()"); }
	catch (const std::exception & e) { throw aException(e.what(), "ak::aIconManager::createMovie()"); }
	catch (...) { throw aException("Unknown error", "ak::aIconManager::createMovie()"); }
}

std::vector<QString> ak::aIconManager::searchDirectories(void) const { return m_directories; }
