//! @file WidgetView.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTGui/WidgetViewCfg.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qstring.h>

// std header
#include <string>

class QWidget;

namespace ads {
	class CDockWidget;
}

namespace ot {

	class OT_WIDGETS_API_EXPORT WidgetView {
		OT_DECL_NOCOPY(WidgetView)
	public:
		WidgetView();
		virtual ~WidgetView();

		// ###########################################################################################################################################################################################################################################################################################################################

		virtual QWidget* getViewWidget(void) = 0;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter/Getter

		virtual bool setupFromConfig(WidgetViewCfg* _config);

		ads::CDockWidget* getViewDockWidget(void) const { return m_dockWidget; };

		void setName(const std::string& _name);
		const std::string& name(void) const { return m_name; };

		void setViewTitle(const QString& _title);
		QString viewTitle(void) const;

		void setInitialiDockLocation(WidgetViewCfg::ViewDockLocation _location) { m_dockLocation = _location; };
		WidgetViewCfg::ViewDockLocation initialDockLocation(void) const { return m_dockLocation; };

	protected:
		void addWidgetToDock(QWidget* _widget);

	private:
		ads::CDockWidget* m_dockWidget;

		std::string m_name;
		ot::WidgetViewCfg::ViewFlags m_flags;
		ot::WidgetViewCfg::ViewDockLocation m_dockLocation;
	};

}