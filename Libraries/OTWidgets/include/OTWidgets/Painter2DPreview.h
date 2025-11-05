// @otlicense
// File: Painter2DPreview.h
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
#include <QtWidgets/qframe.h>

namespace ot {

	class Painter2D;

	class OT_WIDGETS_API_EXPORT Painter2DPreview : public QFrame, public WidgetBase {
		Q_OBJECT
		OT_DECL_NOCOPY(Painter2DPreview)
		OT_DECL_NOMOVE(Painter2DPreview)
		OT_DECL_NODEFAULT(Painter2DPreview)
	public:
		//! @brief Constructor.
		//! @param _parent Pointer to the parent widget.
		explicit Painter2DPreview(QWidget* _parent);

		//! @brief Constructor.
		//! @param _painter Painter to preview.
		//! @param _parent Pointer to the parent widget.
		explicit Painter2DPreview(const Painter2D* _painter, QWidget* _parent);

		virtual ~Painter2DPreview();

		void setFromPainter(const ot::Painter2D* _painter);

		virtual void paintEvent(QPaintEvent* _event) override;

		virtual QWidget* getQWidget() override { return this; };
		virtual const QWidget* getQWidget() const override { return this; };

		void setMaintainAspectRatio(bool _maintain) { m_maintainAspectRatio = _maintain; };
		bool isMaintainAspectRatio() const { return m_maintainAspectRatio; };

	private Q_SLOTS:
		void slotGlobalStyleChanged();

	private:
		bool m_maintainAspectRatio;
		QBrush m_brush;
	};

}