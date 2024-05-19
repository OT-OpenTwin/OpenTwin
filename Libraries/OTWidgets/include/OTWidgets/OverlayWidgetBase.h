//! \file OverlayWidgetBase.h
//! \author Alexander Kuester (alexk95)
//! \date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTGui/GuiTypes.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtWidgets/qframe.h>

namespace ot {

	//! \class OverlayWidgetBase
	class OT_WIDGETS_API_EXPORT OverlayWidgetBase : public QFrame {
		OT_DECL_NOCOPY(OverlayWidgetBase)
		OT_DECL_NODEFAULT(OverlayWidgetBase)
	public:
		OverlayWidgetBase(QWidget* _parent, Alignment _overlayAlignment, const QMargins& _overlayMargins = QMargins());
		virtual ~OverlayWidgetBase();

		void setOverlayMargins(const QMargins& _margins) { m_margins = _margins; this->updateOverlayGeometry(); };
		const QMargins& overlayMargins(void) const { return m_margins; };

	protected:
		virtual bool eventFilter(QObject* _watched, QEvent* _event) override;

	private:
		void updateOverlayGeometry(void);
		
		QMargins m_margins;
		Alignment m_alignment;
		QWidget* m_parent;
	};

}