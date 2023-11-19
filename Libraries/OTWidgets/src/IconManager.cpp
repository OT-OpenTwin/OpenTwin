//! @file IconManager.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/IconManager.h"
#include "OpenTwinCore/Logger.h"

// Qt header
#include <QtCore/qfile.h>
#include <QtGui/qicon.h>
#include <QtGui/qpixmap.h>
#include <QtGui/qmovie.h>

ot::IconManager& ot::IconManager::instance(void) {
	static ot::IconManager g_instance;
	return g_instance;
}

void ot::IconManager::addSearchPath(const QString& _path) {
	if (_path.isEmpty()) {
		OT_LOG_WA("Empty search path provided. Ignoring");
		return;
	}
	QString path = _path;
	path.replace("\\", "/");
	if (!path.endsWith("/")) path.append("/");
	m_searchPaths.append(path);
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