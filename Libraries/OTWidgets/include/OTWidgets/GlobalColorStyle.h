//! @file GlobalColorStyle.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/ColorStyle.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qobject.h>
#include <QtCore/qstringlist.h>

// std header
#include <map>
#include <string>

namespace ot {

	class OT_WIDGETS_API_EXPORT GlobalColorStyle : public QObject {
		Q_OBJECT
		OT_DECL_NOCOPY(GlobalColorStyle)
	public:
		
		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter/Getter

		void addStyle(const ColorStyle& _style, bool _replace = false);
		bool hasStyle(const std::string& _name) const;
		const ColorStyle& getStyle(const std::string& _name, const ColorStyle& _default = ColorStyle()) const;

		bool setCurrentStyle(const std::string& _styleName);
		const std::string& getCurrentStyleName(void) const { return m_currentStyle; };
		const ColorStyle& getCurrentStyle(void) const;

		//! @brief Adds a root path where the style directories should be located at.
		//! Returns false if the specified path does not exist.
		bool addStyleRootSearchPath(const QString& _path);

		QString styleRootPath(const std::string& _styleName) const;

	Q_SIGNALS:
		void currentStyleChanged(const ColorStyle& _style);

	private:
		GlobalColorStyle();
		~GlobalColorStyle() {};

		void evaluateStyleSheetMacros(ColorStyle& _style);

		std::string m_currentStyle;
		ColorStyle m_emptyStyle;
		std::map<std::string, ColorStyle> m_styles;
		QStringList m_styleRootSearchPaths;
	};

}
