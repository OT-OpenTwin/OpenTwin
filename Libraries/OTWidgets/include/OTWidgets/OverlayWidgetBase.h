//! \file OverlayWidgetBase.h
//! \author Alexander Kuester (alexk95)
//! \date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"
#include "OTWidgets/WidgetEventNotifier.h"

namespace ot {

	//! \class OverlayWidgetBase
	//! \brief The OverlayWidgetBase should be used as a base class for an overlay that will be displayed on top of a WidgetEventHandler.
	class OT_WIDGETS_API_EXPORT OverlayWidgetBase : public WidgetEventNotifier {
	public:
		OverlayWidgetBase() {};
		virtual ~OverlayWidgetBase() {};

		virtual void parentWidgetEvent(QEvent* _event) override;

		virtual void overlayParentCreated(void) {};
		virtual void overlayParentDestroyed(void) {};
		virtual void overlayParentMoved(void) {};
		virtual void overlayParentResized(void) {};
		virtual void overlayParentHidden(void) {};
		virtual void overlayParentShown(void) {};
		virtual void overlayParentClosed(void) {};
		virtual void overlayParentQuitted(void) {};
	};

}