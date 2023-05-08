#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtWidgets/qgraphicsscene.h>

namespace ot {

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

	protected:
		virtual void drawBackground(QPainter* _painter, const QRectF& _rect) override;

	private:
		int m_gridSize;
	};

}