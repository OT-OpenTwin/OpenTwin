//! @file GraphicsScene.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/GraphicsConnectionCfg.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtWidgets/qgraphicsscene.h>

namespace ot {

	class GraphicsView;
	class GraphicsItem;
	class GraphicsConnectionPreviewItem;

	//! @brief Graphics Scene for ot::GraphicsItem
	//! Adding QGraphicsItems to the scene that do not inherit ot::GraphicsItem might lead to undefined behavior
	class OT_WIDGETS_API_EXPORT GraphicsScene : public QGraphicsScene {
		Q_OBJECT
	public:
		GraphicsScene(GraphicsView * _view);
		virtual ~GraphicsScene();

		//! @brief Set the grid size
		//! If the size is set to 0 or less, no grid will be drawn
		//! @param _size The grid size to set
		void setGridSize(int _size) { m_gridSize = _size; };

		//! @brief Returns the currently set grid size
		int gridSize(void) const { return m_gridSize; };

		virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* _event) override;
		virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* _event) override;
		virtual void mousePressEvent(QGraphicsSceneMouseEvent* _event) override;
		virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) override;

		//! @brief A connection line according to the current configuration will be drawn with the provided item as origin
		//! @param _item Origin item
		void startConnection(ot::GraphicsItem* _item);
		void stopConnection(void);

		GraphicsView* getGraphicsView(void) { return m_view; };

		void setConnectionPreviewStyle(GraphicsConnectionCfg::ConnectionStyle _style) { m_connectionPreviewStyle = _style; };
		GraphicsConnectionCfg::ConnectionStyle connectionPreviewStyle(void) const { return m_connectionPreviewStyle; };

		void setIgnoreEvents(bool _ignore) { m_ignoreEvents = _ignore; };
		bool ignoreEvents(void) const { return m_ignoreEvents; };

	Q_SIGNALS:
		void selectionChangeFinished(void);

	public Q_SLOTS:
		void slotSelectionChanged(void);
		void handleSelectionChanged(void);

	protected:
		virtual void drawBackground(QPainter* _painter, const QRectF& _rect) override;

	private:
		bool m_ignoreEvents;
		int m_gridSize;
		GraphicsView* m_view;
		GraphicsItem* m_connectionOrigin;
		GraphicsConnectionPreviewItem* m_connectionPreview;
		ot::GraphicsConnectionCfg::ConnectionStyle m_connectionPreviewStyle;
		QList<QGraphicsItem*> m_lastSelection;
		bool m_mouseIsPressed;

		GraphicsScene() = delete;
	};

}
