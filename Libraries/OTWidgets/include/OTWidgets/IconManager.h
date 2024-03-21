//! @file IconManager.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"
#include "OTCore/Logger.h"

// Qt header
#include <qstring.h>
#include <qstringlist.h>

// std header
#include <map>
#include <mutex>

class QIcon;
class QPixmap;
class QMovie;

namespace ot {

	class OT_WIDGETS_API_EXPORT IconManager {
	public:
		static IconManager& instance(void);

		bool addSearchPath(const QString& _path);
		const QStringList& searchPaths(void) const { return m_searchPaths; };

		//! @brief Create and return the icon
		QIcon& getIcon(const QString& _subPath);

		//! @brief Create and return the pixmap
		QPixmap& getPixmap(const QString& _subPath);

		//! @brief Create and return the movie
		QMovie& getMovie(const QString& _subPath);

	private:
		template <class T>
		T& get(const QString& _subPath, std::map<QString, T*>& _dataMap);

		QString findFullPath(const QString& _subPath);

		IconManager();
		virtual ~IconManager();

		QStringList m_searchPaths;
		std::mutex m_mutex;
		std::map<QString, QIcon*> m_icons;
		std::map<QString, QPixmap*> m_pixmaps;
		std::map<QString, QMovie*> m_movies;

	};

}

template <class T>
T& ot::IconManager::get(const QString& _subPath, std::map<QString, T*>& _dataMap) {
	this->m_mutex.lock();

	// Find existing
	auto it = _dataMap.find(_subPath);
	if (it != _dataMap.end()) {
		this->m_mutex.unlock();
		return *it->second;
	}

	// Find new
	QString path = this->findFullPath(_subPath);
	if (path.isEmpty()) {
		this->m_mutex.unlock();
		OT_LOG_EAS("Icon \"" + _subPath.toStdString() + "\" not found");
		throw std::exception("File not found");
	}

	// Create and store new
	T* newEntry = new T(path);
	_dataMap.insert_or_assign(_subPath, newEntry);

	this->m_mutex.unlock();
	return *newEntry;
}