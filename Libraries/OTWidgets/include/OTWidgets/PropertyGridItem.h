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
	class PropertyInput;

	class OT_WIDGETS_API_EXPORT PropertyGridItem : public QTreeWidgetItem {
		OT_DECL_NOCOPY(PropertyGridItem)
	public:
		PropertyGridItem();
		virtual ~PropertyGridItem();

		//! @brief Setup the item
		//! Note that the item must be placed in a tree before calling the setup
		bool setupFromConfig(Property* _config);

		//! @brief Finish the setup (should be called after the item is placed in a tree and after calling setup from config)
		void finishSetup(void);

		PropertyInput* getInput(void) const { return m_input; };

		void setItemBrush(const QBrush& _brush) { m_itemBrush = _brush; };
		const QBrush& itemBrush(void) const { return m_itemBrush; };

	private:
		PropertyInput* m_input;
		QBrush m_itemBrush;
	};

}