// @otlicense
// File: PixmapWidget.h
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
#include <QtGui/qpixmap.h>
#include <QtWidgets/qframe.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT PixmapWidget : public QFrame, public WidgetBase {
		Q_OBJECT
		OT_DECL_NOCOPY(PixmapWidget)
		OT_DECL_NOMOVE(PixmapWidget)
		OT_DECL_NODEFAULT(PixmapWidget)
	public:
		explicit PixmapWidget(QWidget* _parent);
		explicit PixmapWidget(const QPixmap& _pixmap, QWidget* _parent);
		virtual ~PixmapWidget();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Virtual methods

		virtual QWidget* getQWidget(void) override { return this; };
		virtual const QWidget* getQWidget(void) const override { return this; };

		virtual void paintEvent(QPaintEvent* _event) override;

		virtual QSize sizeHint(void) const override;

		virtual void wheelEvent(QWheelEvent* _event) override;

		virtual void keyPressEvent(QKeyEvent* _event) override;

		virtual void mousePressEvent(QMouseEvent* _event) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Pixmap manipulation

		void setPixmap(const QPixmap& _pixmap);
		const QPixmap& getPixmap(void) const { return m_pixmap; };

		void setPixmapResizable(bool _resizable) { m_enabledResizing = _resizable; };
		bool getPixmapResizable(void) const { return m_enabledResizing; };

	Q_SIGNALS:
		void imagePixedClicked(const QPoint& _px);

	protected:
		virtual void resizeEvent(QResizeEvent* _event) override;

		virtual QSize calculateScaledPixmap(const QSize& _newPreferredSize);

	private:
		bool m_enabledResizing;
		bool m_userResized;
		QPixmap m_pixmap;
		QPixmap m_scaledPixmap;
		QSizeF m_scaleFactors;

		void updateScaleFactors(void);
	};

}