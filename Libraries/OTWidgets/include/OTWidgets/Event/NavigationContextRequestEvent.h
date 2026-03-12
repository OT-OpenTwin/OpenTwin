// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/Event/ContextRequestWidgetEvent.h"

// Qt header
#include <QtWidgets/qtreewidget.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT NavigationContextRequestEvent : public ContextRequestWidgetEvent
	{
		OT_DECL_NOCOPY(NavigationContextRequestEvent)
		OT_DECL_NOMOVE(NavigationContextRequestEvent)
		OT_DECL_NODEFAULT(NavigationContextRequestEvent)
	public:
		NavigationContextRequestEvent(ContextMenuCallbackBase* _callbackBase, const QPoint& _pos);
		virtual ~NavigationContextRequestEvent() = default;
		
		//! @brief Create a new instance of the context request data for this event. 
		//! Caller takes ownership of the created instance.
		virtual ContextRequestData* createRequestData(const ContextMenuManagerHandler* _handler) const override;

		QTreeWidget* getTreeWidget() const { return this->getWidgetAs<QTreeWidget>(); };

		QTreeWidgetItem* getItemAt(const QPoint& _pos) const;
		QTreeWidgetItem* getItem() const { return getItemAt(this->getPosition()); };

	};
} // namespace ot