//! @file PropertyGridEntry.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtWidgets/qtreewidget.h>

namespace ot {

	class Property;

	class OT_WIDGETS_API_EXPORT PropertyGridItem : public QTreeWidgetItem {
		OT_DECL_NOCOPY(PropertyGridItem)
	public:
		PropertyGridItem();
		virtual ~PropertyGridItem();

		virtual bool setupFromConfig(Property* _config);

	private:

	};

}