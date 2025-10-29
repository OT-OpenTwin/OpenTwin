// @otlicense

//! @file OverlayWidgetBase.h
//! @author Alexander Kuester (alexk95)
//! @date May 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtWidgets/qframe.h>

namespace ot {

	//! @class OverlayWidgetBase
	class OT_WIDGETS_API_EXPORT OverlayWidgetBase : public QFrame {
		OT_DECL_NOCOPY(OverlayWidgetBase)
		OT_DECL_NODEFAULT(OverlayWidgetBase)
	public:
		OverlayWidgetBase(QWidget* _parent, Alignment _overlayAlignment, const QMargins& _overlayMargins = QMargins());
		virtual ~OverlayWidgetBase();

		void setOverlayMargins(int _left, int _top, int _right, int _bottom) { this->setOverlayMargins(QMargins(_left, _top, _right, _bottom)); };
		void setOverlayMargins(const QMargins& _margins) { m_margins = _margins; this->updateOverlayGeometry(); };
		const QMargins& overlayMargins(void) const { return m_margins; };

		void setOverlayHidden(bool _hidden) { m_hidden = _hidden; this->updateOverlayGeometry(); };

	protected:
		virtual bool eventFilter(QObject* _watched, QEvent* _event) override;

	private:
		void updateOverlayGeometry(void);
		
		bool m_hidden;
		QMargins m_margins;
		Alignment m_alignment;
		QWidget* m_parent;
	};

}