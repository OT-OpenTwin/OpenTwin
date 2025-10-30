// @otlicense

#pragma once

// OpenTwin header
#include "OTWidgets/ManagedPropertyLink.h"

// std header
#include <string>

namespace ot {

	class Property;
	class PropertyGrid;
	
	//! @class PropertyManagerGridLink
	//! @brief The PropertyManagerGridLink is a link between properties of a PropertyManager and a PropertyGrid.
	//! The PropertyManagerGridLink can be visualized at a grid.
	//! When visualized any changes to the properties in the grid will be forwarded to the actual properties synchronizing a grid with a WidgetPropertyManager.
	//! @ref PropertyGrid
	//! @ref WidgetPropertyManager
	class OT_WIDGETS_API_EXPORT PropertyManagerGridLink : public ManagedPropertyLink {
		Q_OBJECT
		OT_DECL_NOCOPY(PropertyManagerGridLink)
		OT_DECL_NOMOVE(PropertyManagerGridLink)
		OT_DECL_NODEFAULT(PropertyManagerGridLink)
	public:
		PropertyManagerGridLink(WidgetPropertyManager* _manager);
		virtual ~PropertyManagerGridLink();

		void visualizeAtGrid(PropertyGrid* _grid);

		void forgetPropertyGrid(void);

	private Q_SLOTS:
		void slotPropertyChanged(const Property* _property);
		void slotPropertyDeleteRequested(const Property* _property);

	private:
		PropertyGrid* m_grid;

	};

}