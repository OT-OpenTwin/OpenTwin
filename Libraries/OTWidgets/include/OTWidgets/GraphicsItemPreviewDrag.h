// @otlicense
// File: GraphicsItemPreviewDrag.h
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
#include "OTCore/BasicServiceInformation.h"
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtCore/qobject.h>

#define OT_GRAPHICSITEMPREVIEWDRAG_MIMETYPE_ItemName "GraphicsItem.Name"
#define OT_GRAPHICSITEMPREVIEWDRAG_MIMETYPE_PickerKey "GraphicsItem.PickerKey"

class QWidget;

namespace ot {

	class OT_WIDGETS_API_EXPORT GraphicsItemPreviewDrag : public QObject {
		Q_OBJECT
		OT_DECL_NODEFAULT(GraphicsItemPreviewDrag)
		OT_DECL_NOCOPY(GraphicsItemPreviewDrag)
		OT_DECL_NOMOVE(GraphicsItemPreviewDrag)
	public:
		GraphicsItemPreviewDrag(const std::string& _itemName, const std::string& _pickerKey);
		virtual ~GraphicsItemPreviewDrag();

		void queue(QWidget* _widget);

	private Q_SLOTS:
		void slotQueue();

	private:
		QWidget* m_widget;
		int m_queueCount;
		std::string m_itemName;
		std::string m_pickerKey;
	};

}
