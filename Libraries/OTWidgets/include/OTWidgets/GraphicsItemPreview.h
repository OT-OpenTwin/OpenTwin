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

		void setOwner(const BasicServiceInformation& _owner) { m_owner = _owner; };
		const BasicServiceInformation& getOwner() const { return m_owner; };

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
		BasicServiceInformation m_owner;

		GraphicsItemPreviewDrag* m_drag;
	};

}