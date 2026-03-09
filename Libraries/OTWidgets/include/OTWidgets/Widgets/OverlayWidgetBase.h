// @otlicense
// File: OverlayWidgetBase.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

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