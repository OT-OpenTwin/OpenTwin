//! @file IconManager.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/IconManager.h"
#include "OTCore/Logger.h"

// Qt header
#include <QtCore/qdir.h>
#include <QtCore/qfile.h>
#include <QtGui/qicon.h>
#include <QtGui/qpixmap.h>
#include <QtGui/qmovie.h>

ot::IconManager& ot::IconManager::instance(void) {
	static ot::IconManager g_instance;
	return g_instance;
}

bool ot::IconManager::addSearchPath(const QString& _path) {
	IconManager& manager = IconManager::instance();
	if (_path.isEmpty()) {
		OT_LOG_WA("Empty search path provided. Ignoring");
		return false;
	}

	QString path = _path;
	path.replace("\\", "/");
	if (!path.endsWith("/")) path.append("/");

	for (const QString& pth : manager.m_searchPaths) {
		if (pth == path) return true;
	}

	QDir dir(path);
	if (dir.exists(path)) {
		manager.m_searchPaths.append(path);
		OT_LOG_D("Added icon search path: \"" + path.toStdString() + "\"");
		return true;
	}
	else {
		OT_LOG_D("Icon search path does not exist. Ignoring. Path: \"" + path.toStdString() + "\"");
		return false;
	}
}

const QStringList& ot::IconManager::searchPaths(void) {
	return IconManager::instance().m_searchPaths;
}

QIcon& ot::IconManager::getIcon(const QString& _subPath) {
	IconManager& manager = IconManager::instance();
	return manager.getOrCreate<QIcon>(_subPath, manager.m_icons, manager.m_emptyIcon);
}

QPixmap& ot::IconManager::getPixmap(const QString& _subPath) {
	IconManager& manager = IconManager::instance();
	return manager.getOrCreate<QPixmap>(_subPath, manager.m_pixmaps, manager.m_emptyPixmap);
}

QMovie& ot::IconManager::getMovie(const QString& _subPath) {
	IconManager& manager = IconManager::instance();
	return manager.getOrCreate<QMovie>(_subPath, manager.m_movies, manager.m_emptyMovie);
}

QByteArray& ot::IconManager::getSvgData(const QString& _subPath) {
	IconManager& manager = IconManager::instance();
	manager.m_mutex.lock();

	// Find existing
	const auto& it = manager.m_svgData.find(_subPath);
	if (it != manager.m_svgData.end()) {
		manager.m_mutex.unlock();
		return it->second;
	}

	// Find new
	QString path = manager.findFullPath(_subPath);
	if (path.isEmpty()) {
		manager.m_mutex.unlock();
		OT_LOG_EAS("Icon \"" + _subPath.toStdString() + "\" not found");
		return manager.m_emptySvgData;
	}

	// Create and store new
	QFile file(path);
	if (!file.open(QIODevice::ReadOnly)) {
		OT_LOG_EAS("Failed to open file for reading: \"" + path.toStdString() + "\"");
		return manager.m_emptySvgData;
	}
	manager.m_svgData.insert_or_assign(_subPath, file.readAll());
	file.close();
	manager.m_mutex.unlock();

	return IconManager::getSvgData(_subPath);
}

void ot::IconManager::setApplicationIcon(const QIcon& _icon) {
	IconManager::instance().m_applicationIcon = _icon;
}

const QIcon& ot::IconManager::getApplicationIcon(void) {
	return IconManager::instance().m_applicationIcon;
}

QString ot::IconManager::findFullPath(const QString& _subPath) {
	for (auto p : m_searchPaths) {
		if (QFile::exists(p + _subPath)) return p + _subPath;
	}
	return QString();
}

ot::IconManager::IconManager() {

}

ot::IconManager::~IconManager() {
	for (auto i : m_icons) delete i.second;
	for (auto i : m_pixmaps) delete i.second;
	for (auto i : m_movies) delete i.second;
}
