//! @file ManagedPropertyLink.h
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtCore/qobject.h>

namespace ot {

	class QWidgetInterface;
	class WidgetPropertyManager;

	//! @class ManagedPropertyLink
	//! @brief The ManagedPropertyLink is a base class to link widget components to managed properties.
	//! @ref PropertyManager
	class OT_WIDGETS_API_EXPORT ManagedPropertyLink : public QObject {
		Q_OBJECT
		OT_DECL_NOCOPY(ManagedPropertyLink)
		OT_DECL_NOMOVE(ManagedPropertyLink)
		OT_DECL_NODEFAULT(ManagedPropertyLink)
	public:
		ManagedPropertyLink(WidgetPropertyManager* _manager);
		virtual ~ManagedPropertyLink();

		WidgetPropertyManager* getWidgetPropertyManager(void) { return m_manager; };
		const WidgetPropertyManager* getWidgetPropertyManager(void) const { return m_manager; };

	private:
		WidgetPropertyManager* m_manager;
	};

}