//! @file WidgetManager.h
//! @author Alexander Kuester (alexk95)
//! @date September 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTObject.h"
#include "OTCore/BasicServiceInformation.h"
#include "OTGui/GuiTypes.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// std header
#include <map>
#include <list>

namespace ot {

	class Widget;

	class OT_WIDGETS_API_EXPORT WidgetManager : public OTObject {
		OT_DECL_NOCOPY(WidgetManager)
		OT_DECL_NOMOVE(WidgetManager)
	public:
		WidgetManager();
		virtual ~WidgetManager();

		void registerWidget(Widget* _widget);
		void deregisterWidget(Widget* _widget);

		void ownerDisconnected(const BasicServiceInformation& _owner);

		Widget* findWidget(const std::string& _name) const;

		void lock(const BasicServiceInformation& _owner, const ot::LockTypeFlags& _lockFlags);
		void unlock(const BasicServiceInformation& _owner, const ot::LockTypeFlags& _lockFlags);

	protected:
		virtual void objectWasDestroyed(OTObject* _object) override;

	private:
		struct OwnerData {
			std::list<Widget*> widgets;
			std::map<LockTypeFlag, int> lockData;
		};

		//! @brief Maps ObjectName to Widget.
		std::map<std::string, Widget*> m_widgets;

		//! @brief Maps BSI to Name to Widgets.
		std::map<BasicServiceInformation, OwnerData> m_ownerData;

		//! @brief Currently set locks.
		std::map<LockTypeFlag, int> m_lockData;

		//! @brief Removes all entries related to the widget.
		void clearWidgetInfo(Widget* _widget);

		OwnerData& findOrCreateOwnerData(const BasicServiceInformation& _owner);

		int& getLockCount(OwnerData& _ownerData, LockTypeFlag _lockFlag);
	};

}
