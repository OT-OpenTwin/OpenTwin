//! \file OverlayWidgetBase.h
//! \author Alexander Kuester (alexk95)
//! \date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTWidgets/OverlayWidgetBase.h"

// Qt header
#include <QtGui/qevent.h>

void ot::OverlayWidgetBase::parentWidgetEvent(QEvent* _event) {
	switch (_event->type())
	{
	case QEvent::Move: 
		this->overlayParentMoved();
		break;
	case QEvent::Resize:
		this->overlayParentResized();
		break;
	case QEvent::Create:
		this->overlayParentCreated();
		break;
	case QEvent::Destroy:
		this->overlayParentDestroyed();
		break;
	case QEvent::Show:
		this->overlayParentShown();
		break;
	case QEvent::Hide:
		this->overlayParentHidden();
		break;
	case QEvent::Close:
		this->overlayParentClosed();
		break;
	case QEvent::Quit:
		this->overlayParentQuitted();
		break;
	default:
		break;
	}
}