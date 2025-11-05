// @otlicense
// File: IndicatorWidget.h
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
#include "OTWidgets/WidgetBase.h"

// Qt header
#include <QtWidgets/qabstractbutton.h>
#include <QtWidgets/qstyle.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT IndicatorWidget : public QAbstractButton, public WidgetBase {
		Q_OBJECT
		OT_DECL_NOCOPY(IndicatorWidget)
		OT_DECL_NOMOVE(IndicatorWidget)
		OT_DECL_NODEFAULT(IndicatorWidget)
		Q_PROPERTY(Symbol checkedSymbol READ getCheckedSymbol WRITE setCheckedSymbol)
		Q_PROPERTY(Symbol uncheckedSymbol READ getUncheckedSymbol WRITE setUncheckedSymbol)
		Q_PROPERTY(bool drawFrame READ getDrawFrame WRITE setDrawFrame)
	public:
		enum Symbol {
			None,
			Checkmark,
			Cross,
			Circle
		};
		Q_ENUM(Symbol)

		explicit IndicatorWidget(QWidget* _parent);
		explicit IndicatorWidget(bool _checked, QWidget* _parent);
		virtual ~IndicatorWidget() = default;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Public: Vitual methods

		virtual QWidget* getQWidget(void) override { return this; };
		virtual const QWidget* getQWidget(void) const override { return this; };

		QSize sizeHint() const override;
		QSize minimumSizeHint() const override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setCheckedSymbol(Symbol _symbol);
		Symbol getCheckedSymbol() const { return m_checkedSymbol; };

		void setUncheckedSymbol(Symbol _symbol);
		Symbol getUncheckedSymbol() const { return m_uncheckedSymbol; };

		void setDrawFrame(bool enable);
		bool getDrawFrame() const { return m_drawFrame; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Protected: Overridden methods

	protected:
		void paintEvent(QPaintEvent* event) override;

		void enterEvent(QEnterEvent* event) override {
			QAbstractButton::enterEvent(event);
			update();
		}

		void leaveEvent(QEvent* event) override {
			QAbstractButton::leaveEvent(event);
			update();
		}

		void mousePressEvent(QMouseEvent* event) override {
			QAbstractButton::mousePressEvent(event);
			update();
		}

		void mouseReleaseEvent(QMouseEvent* event) override {
			QAbstractButton::mouseReleaseEvent(event);
			update();
		}

	private:
		QStyle::State getButtonState() const;

		Symbol m_checkedSymbol;
		Symbol m_uncheckedSymbol;
		bool m_drawFrame;
	};
}

