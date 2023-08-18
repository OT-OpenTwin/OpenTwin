//! @file GraphicsScene.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtWidgets/qgraphicsscene.h>

namespace ot {

	class GraphicsItem;

	class OT_WIDGETS_API_EXPORT GraphicsScene : public QGraphicsScene {
		Q_OBJECT
	public:
		GraphicsScene();
		virtual ~GraphicsScene();

		//! @brief Set the grid size
		//! If the size is set to 0 or less, no grid will be drawn
		//! @param _size The grid size to set
		void setGridSize(int _size) { m_gridSize = _size; };

		//! @brief Returns the currently set grid size
		int gridSize(void) const { return m_gridSize; };

		virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* _event) override;

		//! @brief A connection line according to the current configuration will be drawn with the provided item as origin
		//! @param _item Origin item
		void startConnection(ot::GraphicsItem* _item);

	protected:
		virtual void drawBackground(QPainter* _painter, const QRectF& _rect) override;

	private:
		int m_gridSize;
		QGraphicsItem* m_connectionOrigin;
	};

}