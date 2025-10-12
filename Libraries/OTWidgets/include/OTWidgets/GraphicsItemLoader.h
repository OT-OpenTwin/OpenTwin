//! @file GraphicsItemLoader.h
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

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
		GraphicsItemLoader();

		//! @brief Private destructor.
		virtual ~GraphicsItemLoader();

		//! @brief Returns the first existing search path +  sub path combination.
		//! @param _subPath The sub path that will be appended to a search path.
		QString findFullItemPath(const QString& _subPath) const;

		std::list<QString> m_searchPaths;
		std::map<QString, GraphicsItemCfg*> m_configurations;
	};

}