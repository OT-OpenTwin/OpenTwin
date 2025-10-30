// @otlicense
// File: SVGWidget.h
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

class QSvgWidget;

namespace ot {

	class OT_WIDGETS_API_EXPORT SVGWidget : public QObject, public WidgetBase {
		Q_OBJECT
	public:
		SVGWidget(QWidget* _parent = (QWidget*)nullptr) : SVGWidget(QByteArray(), _parent) {};
		SVGWidget(const QByteArray& _svgData, QWidget* _parent = (QWidget*)nullptr);

		virtual QWidget* getQWidget(void) override;
		virtual const QWidget* getQWidget(void) const override;

		void setSvgData(const QByteArray& _data);
		const QByteArray& getSvgData(void) const { return m_data; };

		void setFixedSize(int _width, int _height) { this->setFixedSize(QSize(_width, _height)); };
		void setFixedSize(const QSize& _size);

		void loadFromFile(const QString& _filePath);

	private:
		QSvgWidget* m_widget;
		QByteArray m_data;
	};

}