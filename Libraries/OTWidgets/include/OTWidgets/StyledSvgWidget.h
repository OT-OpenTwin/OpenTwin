// @otlicense
// File: StyledSvgWidget.h
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

	class SvgImagePainter;

	class OT_WIDGETS_API_EXPORT StyledSvgWidget : public QFrame, public WidgetBase {
		Q_OBJECT
		OT_DECL_NOCOPY(StyledSvgWidget)
		OT_DECL_NOMOVE(StyledSvgWidget)
		OT_DECL_NODEFAULT(StyledSvgWidget)
	public:
		StyledSvgWidget(const QString& _fileSubPath, QWidget* _parent);
		virtual ~StyledSvgWidget();

		virtual QWidget* getQWidget() override { return this; };
		virtual const QWidget* getQWidget() const override { return this; };

		void setMaintainAspectRatio(bool _enable) { m_maintainAspectRatio = _enable; };
		bool getMaintainAspectRatio() const { return m_maintainAspectRatio; };

		virtual QSize sizeHint() const override;

	protected:
		void paintEvent(QPaintEvent* _event) override;

	private Q_SLOTS:
		void slotStyleChanged();

	private:
		bool m_maintainAspectRatio;
		QString m_fileSubPath;
		SvgImagePainter* m_svgPainter;

	};

}

