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
	if (_path.isEmpty()) {
		OT_LOG_WA("Empty search path provided. Ignoring");
		return false;
	}

	QString path = _path;
	path.replace("\\", "/");
	if (!path.endsWith("/")) path.append("/");

	for (const QString& pth : m_searchPaths) {
		if (pth == path) return true;
	}

	QDir dir(path);
	if (dir.exists(path)) {
		m_searchPaths.append(path);
		OT_LOG_D("Added icon search path: \"" + path.toStdString() + "\"");
		return true;
	}
	else {
		OT_LOG_D("Icon search path does not exist. Ignoring...");
		return false;
	}
}

QIcon& ot::IconManager::getIcon(const QString& _subPath) {
	return this->get<QIcon>(_subPath, m_icons);
}

QPixmap& ot::IconManager::getPixmap(const QString& _subPath) {
	return this->get<QPixmap>(_subPath, m_pixmaps);
}

QMovie& ot::IconManager::getMovie(const QString& _subPath) {
	return this->get<QMovie>(_subPath, m_movies);
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