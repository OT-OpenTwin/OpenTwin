//! @file WidgetViewManager.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTCore/BasicServiceInformation.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// std header
#include <map>
#include <string>

namespace ot {

	class WidgetView;
	class WidgetViewCfg;

	class WidgetViewManager {
		OT_DECL_NOCOPY(WidgetViewManager)
	public:
		enum DefaultWidgetViewType {
			ProjectNavigationViewType,
			PropertyGridViewType,
			OutputWindowViewType,
			GraphicsPickerViewType
		};

		static WidgetViewManager& instance(void);

		// ###########################################################################################################################################################################################################################################################################################################################

		bool addView(const BasicServiceInformation& _owner, WidgetView* _view);
		
		WidgetView* addView(const BasicServiceInformation& _owner, WidgetViewCfg* _viewConfiguration);

		WidgetView* findView(const BasicServiceInformation& _owner, const std::string& _viewName);

		void closeView(const BasicServiceInformation& _owner, const std::string& _viewName);

		void closeViews(const BasicServiceInformation& _owner);

		WidgetView* findOrCreateDefaultView(DefaultWidgetViewType _viewType);

	private:
		WidgetViewManager();
		~WidgetViewManager();

		//! @brief Clear the maps.
		//! Does not free the views!
		void clear(const BasicServiceInformation& _owner);

		std::map<std::string, WidgetView*>* findViewMap(const BasicServiceInformation& _owner);
		std::map<std::string, WidgetView*>* findOrCreateViewMap(const BasicServiceInformation& _owner);

		std::map<BasicServiceInformation, std::map<std::string, WidgetView*>*> m_views;
		std::map<DefaultWidgetViewType, WidgetView*> m_defaultViews;

	};

}
