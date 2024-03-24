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

	class WidgetViewManager;
	
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

		void setViewIsProtected(bool _protected = true) { m_isProtected = _protected; };
		bool viewIsProtected(void) const { return m_isProtected; };

		void setViewContentModified(bool _isModified);
		bool isViewContentModified(void) const { return m_isModified; };

		void setName(const std::string& _name);
		const std::string& name(void) const { return m_name; };

		void setViewTitle(const QString& _title);
		QString viewTitle(void) const { return m_title; };
		QString currentViewTitle(void) const;

		void setInitialiDockLocation(WidgetViewCfg::ViewDockLocation _location) { m_dockLocation = _location; };
		WidgetViewCfg::ViewDockLocation initialDockLocation(void) const { return m_dockLocation; };

	protected:
		void addWidgetToDock(QWidget* _widget);

	private:
		friend class WidgetViewManager;

		ads::CDockWidget* m_dockWidget;

		bool m_isDeletedByManager;
		bool m_isProtected;
		bool m_isModified;
		std::string m_name;
		QString m_title;
		ot::WidgetViewCfg::ViewFlags m_flags;
		ot::WidgetViewCfg::ViewDockLocation m_dockLocation;
	};

}