//! @file IconManager.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"
#include "OTCore/LogDispatcher.h"

// Qt header
#include <QtCore/qstring.h>
#include <QtCore/qbytearray.h>
#include <QtCore/qstringlist.h>
#include <QtGui/qicon.h>
#include <QtGui/qmovie.h>
#include <QtGui/qpixmap.h>

// std header
#include <map>
#include <mutex>
#include <optional>

namespace ot {

	//! @class IconManager
	//! @brief The IconManager loads images or animations and caches them.
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

		//! @brief Check if a file exists in any of the search paths.
		//! @param _subPath The sub path of the file to check.
		static bool fileExists(const QString& _subPath);

		//! @brief Return or import and return the icon.
		static const QIcon& getIcon(const QString& _subPath);

		//! @brief Return or import and return the pixmap.
		static const QPixmap& getPixmap(const QString& _subPath);

		//! @brief Return or import and return the movie.
		static std::shared_ptr<QMovie> getMovie(const QString& _subPath);

		//! @brief Return or import and return the pixmap.
		static const QByteArray& getSvgData(const QString& _subPath);

		//! @brief Stores the application icon.
		static void setApplicationIcon(const QIcon& _icon);

		//! @brief Returns the main application icon.
		//! This icon may be used for windows and dialogs to let them all have the same icon.
		static const QIcon& getApplicationIcon(void);

	private:
		template <class T>
		std::shared_ptr<T>& getOrCreate(const QString& _subPath, std::map<QString, std::shared_ptr<T>>& _dataMap, std::shared_ptr<T>& _default);

		template <class T>
		std::optional<std::shared_ptr<T>*> get(const QString& _subPath, std::map<QString, std::shared_ptr<T>>& _dataMap);

		template <class T>
		std::optional<std::shared_ptr<T>*> getWhileLocked(const QString& _subPath, std::map<QString, std::shared_ptr<T>>& _dataMap);

		QString findFullPath(const QString& _subPath);

		IconManager();
		virtual ~IconManager();

		QStringList m_searchPaths;
		std::mutex m_mutex;
		std::map<QString, std::shared_ptr<QIcon>> m_icons;
		std::map<QString, std::shared_ptr<QPixmap>> m_pixmaps;
		std::map<QString, std::shared_ptr<QMovie>> m_movies;
		std::map<QString, std::shared_ptr<QByteArray>> m_svgData;
		std::shared_ptr<QIcon> m_emptyIcon;
		std::shared_ptr<QPixmap> m_emptyPixmap;
		std::shared_ptr<QMovie> m_emptyMovie;
		std::shared_ptr<QByteArray> m_emptySvgData;
		QIcon m_applicationIcon;
	};

}

#include "OTWidgets/IconManager.hpp"