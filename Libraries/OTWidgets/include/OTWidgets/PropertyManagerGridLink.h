//! @file PropertyManagerGridLink.h
//! @author Alexander Kuester (alexk95)
//! @date March 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/ManagedPropertyLink.h"

// std header
#include <string>

namespace ot {

	class Property;
	class PropertyGrid;
	
	class OT_WIDGETS_API_EXPORT PropertyManagerGridLink : public ManagedPropertyLink {
		Q_OBJECT
		OT_DECL_NOCOPY(PropertyManagerGridLink)
		OT_DECL_NOMOVE(PropertyManagerGridLink)
	public:
		PropertyManagerGridLink(WidgetPropertyManager* _manager = (WidgetPropertyManager*)nullptr);
		virtual ~PropertyManagerGridLink();

		virtual void visualizeProperty(QWidgetInterface* _widget) override;

		void forgetPropertyGrid(void);

	private Q_SLOTS:
		void slotPropertyChanged(const Property* const _property);
		void slotPropertyDeleteRequested(const Property* const _property);

	private:
		PropertyGrid* m_grid;

	};

}