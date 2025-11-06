// @otlicense
// File: IconManager.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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

		//! @brief Return or import and return the svg data.
		static const QByteArray& getSvgData(const QString& _subPath);

		//! @brief Return or import, parse and return the svg data.
		//! @param _subPath The sub path of the svg file to load and parse.
		static const QByteArray& getParsedSvgData(const QString& _subPath);

		//! @brief Stores the application icon.
		static void setApplicationIcon(const QIcon& _icon);

		//! @brief Returns the main application icon.
		//! This icon may be used for windows and dialogs to let them all have the same icon.
		static const QIcon& getApplicationIcon(void);

		//! @brief Stores the default project icon.
		//! @param _icon The icon to use as default project icon.
		static void setDefaultProjectIcon(const QIcon& _icon);

		//! @brief Returns the default project icon.
		static const QIcon& getDefaultProjectIcon(void);

		static void clearParsedSvgDataCache();

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
		std::map<QString, QByteArray> m_parsedSvgData;
		std::shared_ptr<QIcon> m_emptyIcon;
		std::shared_ptr<QPixmap> m_emptyPixmap;
		std::shared_ptr<QMovie> m_emptyMovie;
		QByteArray m_emptyByteArray;
		QIcon m_applicationIcon;
		QIcon m_defaultProjectIcon;
	};

}

#include "OTWidgets/IconManager.hpp"