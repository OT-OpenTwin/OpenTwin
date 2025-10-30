// @otlicense
// File: SVGWidgetGrid.h
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
#include <QtCore/qtimer.h>
#include <QtWidgets/qscrollarea.h>

// std header
#include <vector>

class QGridLayout;

namespace ot {

	class SVGWidget;

	class OT_WIDGETS_API_EXPORT SVGWidgetGrid : public QScrollArea, public WidgetBase {
		Q_OBJECT
	public:
		SVGWidgetGrid();

		void fillFromPath(const QString& _rootPath);

		void clear(void);

		virtual QWidget* getQWidget(void) override { return this; };

		virtual const QWidget* getQWidget(void) const override { return this; };

		void setItemSize(int _widthAndHeight) { this->setItemSize(QSize(_widthAndHeight, _widthAndHeight)); };
		void setItemSize(int _width, int _height) { this->setItemSize(QSize(_width, _height)); };
		void setItemSize(const QSize& _size);

	protected:
		virtual void resizeEvent(QResizeEvent* _event) override;

	private Q_SLOTS:
		void slotLoadNext(void);

	private:
		void rebuildGrid(const QSize& _newSize);
		
		QGridLayout* m_layout;

		QSize m_itemSize;

		QTimer m_delayedLoadTimer;
		
		struct LoadData {
			size_t ix;
			std::vector<QString> paths;
		};

		LoadData m_loadData;

		std::vector<SVGWidget*> m_widgets;
	};

}