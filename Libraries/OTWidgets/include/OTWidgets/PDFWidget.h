// @otlicense
// File: PDFWidget.h
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
#include <QtCore/qobject.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT PDFWidget : public QObject, public ot::WidgetBase {
		Q_OBJECT
		OT_DECL_NOCOPY(PDFWidget)
		OT_DECL_NOMOVE(PDFWidget)
	public:
		PDFWidget(QWidget* _parentWidget = (QWidget*)nullptr);
		virtual ~PDFWidget();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Virtual methods

		virtual QWidget* getQWidget() override { return m_rootWidget; };
		virtual const QWidget* getQWidget() const override { return m_rootWidget; };

	private:
		QWidget* m_rootWidget;
	};

}