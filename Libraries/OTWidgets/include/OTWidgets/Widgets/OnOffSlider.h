// @otlicense
// File: OnOffSlider.h
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
#include "OTWidgets/Widgets/WidgetBase.h"

// Qt header
#include <QtWidgets/qwidget.h>

class QPropertyAnimation;

namespace ot {

	class OT_WIDGETS_API_EXPORT OnOffSlider : public QWidget {
		Q_OBJECT
		Q_PROPERTY(qreal knobPos READ getKnobPos WRITE setKnobPos)
	public:
		explicit OnOffSlider(QWidget* _parent = (QWidget*)nullptr);

		void setChecked(bool _checked);
		bool getChecked() const { return m_checked; };

		void setShowLabels(bool _on);
		bool getShowLabels() const { return m_showLabels; };

		QSize sizeHint() const override { return { 40, 18 }; };

	Q_SIGNALS:
		void toggled(bool _checked);

	public Q_SLOTS:
		void toggle();

	protected:
		void mousePressEvent(QMouseEvent* _event) override;
		void paintEvent(QPaintEvent* _event) override;


	private:
		void startAnimation();

		void setKnobPos(qreal _pos);
		qreal getKnobPos() const { return m_knobPos; };

	private:
		bool m_checked;
		bool m_showLabels;
		qreal m_knobPos;
		QPropertyAnimation* m_animation;
	};

}