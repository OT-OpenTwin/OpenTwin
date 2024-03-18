//! @file GraphicsItemPreview.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/GraphicsItem.h"

// Qt header
#include <QtWidgets/qlabel.h>

namespace ot {

	class GraphicsItemPreviewDrag;

	class GraphicsItemPreview : public QLabel {
	public:
		GraphicsItemPreview();
		virtual ~GraphicsItemPreview();

		virtual void mousePressEvent(QMouseEvent* _event) override;
		virtual void enterEvent(QEnterEvent* _event) override;
		virtual void leaveEvent(QEvent* _event) override;

		void setItemName(const std::string& _itemName) { m_itemName = _itemName; };
		const std::string itemName(void) const { return m_itemName; };

	private:
		std::string m_itemName;
		GraphicsItemPreviewDrag* m_drag;
	};

}