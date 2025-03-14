//! @file GraphicsItemPreview.h
//! 
//! @author Alexander Kuester (alexk95)
//! @date March 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/BasicServiceInformation.h"
#include "OTWidgets/GraphicsItem.h"

// Qt header
#include <QtWidgets/qlabel.h>

namespace ot {

	class GraphicsItemPreviewDrag;

	class OT_WIDGETS_API_EXPORT GraphicsItemPreview : public QLabel {
	public:
		GraphicsItemPreview();
		virtual ~GraphicsItemPreview();

		virtual void mousePressEvent(QMouseEvent* _event) override;
		virtual void enterEvent(QEnterEvent* _event) override;
		virtual void leaveEvent(QEvent* _event) override;

		void setItemName(const std::string& _itemName) { m_itemName = _itemName; };
		const std::string itemName(void) const { return m_itemName; };

		void setOwner(const BasicServiceInformation& _owner) { m_owner = _owner; };
		const BasicServiceInformation& getOwner(void) const { return m_owner; };

	private:
		std::string m_itemName;
		BasicServiceInformation m_owner;

		GraphicsItemPreviewDrag* m_drag;
	};

}