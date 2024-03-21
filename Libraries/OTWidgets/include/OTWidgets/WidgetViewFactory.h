//! @file WidgetViewFactory.h
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// std header
#include <map>
#include <string>
#include <functional>

namespace ot {

	class WidgetView;
	class WidgetViewCfg;

	using WidgetViewConstructor = std::function<WidgetView* ()>;

	class OT_WIDGETS_API_EXPORT WidgetViewFactory {
		OT_DECL_NOCOPY(WidgetViewFactory)
	public:
		static WidgetView* createView(WidgetViewCfg* _viewConfiguration);

		static WidgetView* createView(const std::string& _viewType);

		template <class T>
		static T* createView(const std::string& _viewType);

		static void registerWidgetViewConstructor(const std::string& _key, const WidgetViewConstructor& _constructor);

	private:
		static std::map<std::string, WidgetViewConstructor>& constructors();

		WidgetViewFactory() {};
		~WidgetViewFactory() {};

	};

}

template <class T>
T* ot::WidgetViewFactory::createView(const std::string& _viewType) {
	WidgetView* view = WidgetViewFactory::createView(_viewType);
	T* newView = dynamic_cast<T*>(view);
	if (!newView) {
		OT_LOG_E("WidgetView cast failed");
		delete view;
		return nullptr;
	}
	return newView;
}