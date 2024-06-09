//! @file IconManager.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"
#include "OTCore/Logger.h"

// Qt header
#include <QtCore/qstring.h>
#include <QtCore/qstringlist.h>
#include <QtGui/qicon.h>
#include <QtGui/qmovie.h>
#include <QtGui/qpixmap.h>

// std header
#include <map>
#include <mutex>

namespace ot {

	//! \class IconManager
	//! \brief The IconManager loads images or animations and caches them.
	//! The IconManager has a search path list.
	//! When requestiong a file only the subPath (suffix) needs to be provided.
	//! When checking every search path and suffix combination the first existing file (FIFO) will be imported.
	//! The imported files are stored internally and will returned when requesting the same file.
	class OT_WIDGETS_API_EXPORT IconManager {
	public:
		//! @brief Returns the global instance.
		static IconManager& instance(void);

		static bool addSearchPath(const QString& _path);

		static const QStringList& searchPaths(void);

		//! @brief Create and return the icon
		static QIcon& getIcon(const QString& _subPath);

		//! @brief Create and return the pixmap
		static QPixmap& getPixmap(const QString& _subPath);

		//! @brief Create and return the movie
		static QMovie& getMovie(const QString& _subPath);

		//! \brief Stores the application icon.
		static void setApplicationIcon(const QIcon& _icon);

		//! \brief Returns the main application icon.
		//! This icon may be used for windows and dialogs to let them all have the same icon.
		static const QIcon& getApplicationIcon(void);

	private:
		template <class T>
		T& getOrCreate(const QString& _subPath, std::map<QString, T*>& _dataMap, T& _default);

		QString findFullPath(const QString& _subPath);

		IconManager();
		virtual ~IconManager();

		QStringList m_searchPaths;
		std::mutex m_mutex;
		std::map<QString, QIcon*> m_icons;
		std::map<QString, QPixmap*> m_pixmaps;
		std::map<QString, QMovie*> m_movies;
		QIcon m_emptyIcon;
		QPixmap m_emptyPixmap;
		QMovie m_emptyMovie;
		QIcon m_applicationIcon;
	};

}

template <class T>
T& ot::IconManager::getOrCreate(const QString& _subPath, std::map<QString, T*>& _dataMap, T& _default) {
	this->m_mutex.lock();

	// Find existing
	const auto& it = _dataMap.find(_subPath);
	if (it != _dataMap.end()) {
		this->m_mutex.unlock();
		return *it->second;
	}

	// Find new
	QString path = this->findFullPath(_subPath);
	if (path.isEmpty()) {
		this->m_mutex.unlock();
		OT_LOG_EAS("Icon \"" + _subPath.toStdString() + "\" not found");
		return _default;
	}

	// Create and store new
	T* newEntry = new T(path);
	_dataMap.insert_or_assign(_subPath, newEntry);

	this->m_mutex.unlock();
	return *newEntry;
}