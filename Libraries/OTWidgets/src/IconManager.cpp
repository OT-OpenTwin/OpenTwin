// @otlicense

//! @file IconManager.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/IconManager.h"
#include "OTCore/LogDispatcher.h"

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

bool ot::IconManager::fileExists(const QString& _subPath) {
	IconManager& manager = IconManager::instance();

	std::lock_guard<std::mutex> lock(manager.m_mutex);

	QString path = manager.findFullPath(_subPath);
	return !path.isEmpty();
}

const QIcon& ot::IconManager::getIcon(const QString& _subPath) {
	IconManager& manager = IconManager::instance();
	return *manager.getOrCreate<QIcon>(_subPath, manager.m_icons, manager.m_emptyIcon);
}

const QPixmap& ot::IconManager::getPixmap(const QString& _subPath) {
	IconManager& manager = IconManager::instance();
	return *manager.getOrCreate<QPixmap>(_subPath, manager.m_pixmaps, manager.m_emptyPixmap);
}

std::shared_ptr<QMovie> ot::IconManager::getMovie(const QString& _subPath) {
	IconManager& manager = IconManager::instance();
	return manager.getOrCreate<QMovie>(_subPath, manager.m_movies, manager.m_emptyMovie);
}

const QByteArray& ot::IconManager::getSvgData(const QString& _subPath) {
	IconManager& manager = IconManager::instance();
	std::lock_guard<std::mutex> lock(manager.m_mutex);

	// Find existing
	std::optional<std::shared_ptr<QByteArray>*> existing = manager.getWhileLocked<QByteArray>(_subPath, manager.m_svgData);
	if (existing.has_value()) {
		if (existing.value()) {
			return *(*existing.value());
		}
	}

	// Find new
	QString path = manager.findFullPath(_subPath);
	if (path.isEmpty()) {
		OT_LOG_E("Icon \"" + _subPath.toStdString() + "\" not found");
		return *manager.m_emptySvgData;
	}

	// Create and store new
	QFile file(path);
	if (!file.open(QIODevice::ReadOnly)) {
		OT_LOG_E("Failed to open file for reading: \"" + path.toStdString() + "\"");
		return *manager.m_emptySvgData;
	}
	
	std::shared_ptr<QByteArray> newData = std::shared_ptr<QByteArray>(new QByteArray(file.readAll()));
	file.close();
	return *manager.m_svgData.insert_or_assign(_subPath, newData).first->second;
}

void ot::IconManager::setApplicationIcon(const QIcon& _icon) {
	IconManager::instance().m_applicationIcon = _icon;
}

const QIcon& ot::IconManager::getApplicationIcon(void) {
	return IconManager::instance().m_applicationIcon;
}

void ot::IconManager::setDefaultProjectIcon(const QIcon& _icon) {
	IconManager::instance().m_defaultProjectIcon = _icon;
}

const QIcon& ot::IconManager::getDefaultProjectIcon(void) {
	return IconManager::instance().m_defaultProjectIcon;
}

QString ot::IconManager::findFullPath(const QString& _subPath) {
	for (auto p : m_searchPaths) {
		if (QFile::exists(p + _subPath)) return p + _subPath;
	}
	return QString();
}

ot::IconManager::IconManager() :
	m_emptyIcon(new QIcon), m_emptyMovie(new QMovie), m_emptyPixmap(new QPixmap), m_emptySvgData(new QByteArray)
{

}

ot::IconManager::~IconManager() {

}
