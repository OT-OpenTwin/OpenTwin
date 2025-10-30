// @otlicense
// File: GraphicsItemLoader.h
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
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtCore/qstring.h>

// std header
#include <map>
#include <list>

namespace ot {

	class GraphicsItemCfg;

	//! @class GraphicsItemLoader
	//! @brief The GraphicsItemLoader may be used to import a GraphicsItemCfg from a file.
	//! The imported configuration(s) will be cached.
	class OT_WIDGETS_API_EXPORT GraphicsItemLoader {
		OT_DECL_NOCOPY(GraphicsItemLoader)
		OT_DECL_NOMOVE(GraphicsItemLoader)
	public:
		//! @brief Returns the global instance.
		static GraphicsItemLoader& instance(void);

		// ###########################################################################################################################################################################################################################################################################################################################

		//! @brief Adds the provided path to the search paths list if valid.
		//! Before adding the path the string will be checked that it contains an existing path.
		//! The search path should end with a '/' (The suffix will be forced).
		//! @param _path Search path to add.
		bool addSearchPath(const QString& _path);

		//! @brief Returns the search paths.
		const std::list<QString>& getSearchPaths(void) const { return m_searchPaths; };

		//! @fn createConfiguration
		//! @brief Imports and caches the specified GraphicsItemCfg.
		//! The first existing search path + sub path combination will be used.
		GraphicsItemCfg* createConfiguration(const QString& _subPath);

		// ###########################################################################################################################################################################################################################################################################################################################

	private:

		//! @brief Private constructor.
		GraphicsItemLoader() = default;

		//! @brief Private destructor.
		virtual ~GraphicsItemLoader() = default;

		//! @brief Returns the first existing search path +  sub path combination.
		//! @param _subPath The sub path that will be appended to a search path.
		QString findFullItemPath(const QString& _subPath) const;

		std::list<QString> m_searchPaths;
		std::map<QString, GraphicsItemCfg*> m_configurations;
	};

}