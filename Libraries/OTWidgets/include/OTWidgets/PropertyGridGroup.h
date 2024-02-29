//! @file PropertyGridGroup.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/PropertyGroup.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtWidgets/qtreewidget.h>

namespace ot {

	class PropertyGridItem;

	class OT_WIDGETS_API_EXPORT PropertyGridGroup : public QTreeWidgetItem {
		OT_DECL_NOCOPY(PropertyGridGroup)
	public:
		PropertyGridGroup();
		virtual ~PropertyGridGroup();

		void setupFromConfig(PropertyGroup* _group);

		void addItem(PropertyGridItem* _item);

	private:

	};

}