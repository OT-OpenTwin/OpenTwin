// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/Event/MenuRequestWidgetEvent.h"

// Qt header
#include <QtWidgets/qtreewidget.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT NavigationMenuRequestEvent : public MenuRequestWidgetEvent
	{
		OT_DECL_NOCOPY(NavigationMenuRequestEvent)
		OT_DECL_NOMOVE(NavigationMenuRequestEvent)
		OT_DECL_NODEFAULT(NavigationMenuRequestEvent)
	public:
		NavigationMenuRequestEvent(MenuCallbackBase* _callbackBase, const QPoint& _pos);
		virtual ~NavigationMenuRequestEvent() = default;
		
		//! @brief Create a new instance of the menu request data for this event. 
		//! Caller takes ownership of the created instance.
		OT_DECL_NODISCARD virtual MenuRequestData* createRequestData(const MenuManagerHandler* _handler) const override;

		OT_DECL_NODISCARD QTreeWidget* getTreeWidget() const { return this->getWidgetAs<QTreeWidget>(); };

		OT_DECL_NODISCARD QTreeWidgetItem* getItemAt(const QPoint& _pos) const;
		OT_DECL_NODISCARD QTreeWidgetItem* getItem() const { return getItemAt(this->getPosition()); };

	};
} // namespace ot