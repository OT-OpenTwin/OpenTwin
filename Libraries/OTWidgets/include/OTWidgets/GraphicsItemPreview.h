// @otlicense
// File: GraphicsItemPreview.h
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
#include "OTWidgets/GraphicsItem.h"

// Qt header
#include <QtWidgets/qframe.h>

namespace ot {

	class ImagePainter;
	class GraphicsItemPreviewDrag;

	class OT_WIDGETS_API_EXPORT GraphicsItemPreview : public QFrame {
		OT_DECL_NOCOPY(GraphicsItemPreview)
		OT_DECL_NOMOVE(GraphicsItemPreview)
	public:
		GraphicsItemPreview();
		virtual ~GraphicsItemPreview();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setItemName(const std::string& _itemName) { m_itemName = _itemName; };
		const std::string getItemName() const { return m_itemName; };

		void setPickerKey(const std::string& _pickerKey) { m_pickerKey = _pickerKey; };
		const std::string getPickerKey() const { return m_pickerKey; };

		void setPainter(ImagePainter* _painter);
		const ImagePainter* getPainter() const { return m_painter; };

		virtual QSize sizeHint() const override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Proetected: Events

	protected:
		virtual void paintEvent(QPaintEvent* _event) override;
		virtual void mousePressEvent(QMouseEvent* _event) override;
		virtual void enterEvent(QEnterEvent* _event) override;
		virtual void leaveEvent(QEvent* _event) override;

	private:
		ImagePainter* m_painter;

		std::string m_itemName;
		std::string m_pickerKey;

		GraphicsItemPreviewDrag* m_drag;
	};

}