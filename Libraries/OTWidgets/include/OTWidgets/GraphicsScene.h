//! @file GraphicsScene.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Flags.h"
#include "OTGui/Grid.h"
#include "OTGui/GraphicsConnectionCfg.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtWidgets/qgraphicsscene.h>

namespace ot {

	class GraphicsBase;
	class GraphicsView;
	class GraphicsItem;
	class GraphicsConnectionItem;
	class GraphicsConnectionPreviewItem;

	//! @brief Graphics Scene for ot::GraphicsItem
	//! Adding QGraphicsItems to the scene that do not inherit ot::GraphicsItem might lead to undefined behavior
	class OT_WIDGETS_API_EXPORT GraphicsScene : public QGraphicsScene {
		Q_OBJECT
	public:
		GraphicsScene(GraphicsView* _view);
		GraphicsScene(const QRectF& _sceneRect, GraphicsView* _view);
		virtual ~GraphicsScene();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Connection handling

		//! @brief A connection line according to the current configuration will be drawn with the provided item as origin
		//! @param _item Origin item
		void startConnection(ot::GraphicsItem* _item);

		//! \brief Will request a connection from the currently set origin to the provided target connection.
		//! The new control point is pos and should be the mouse press position.
		//! \param _targetConnection The destination for the connection request.
		//! \param _pos The new control point position.
		void startConnectionToConnection(ot::GraphicsConnectionItem* _targetedConnection, const Point2DD& _pos);

		void stopConnection(void);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setGrid(const Grid& _grid) { m_grid = _grid; };
		const Grid& getGrid(void) const { return m_grid; };

		void setGridStep(int _step) { m_grid.setGridStep(_step); };
		void setGridStep(const Point2D& _step) { m_grid.setGridStep(_step); };
		Point2D getGridStep(void) const { return m_grid.getGridStep(); };

		void setWideGridLineCounter(int _counter) { m_grid.setWideGridLineCounter(_counter); };
		void setWideGridLineCounter(const Point2D& _counter) { m_grid.setWideGridLineCounter(_counter); };
		Point2D getWideGridLineCounter(void) const { return m_grid.getWideGridLineCounter(); };

		void setGridFlag(Grid::GridFlag _flag, bool _active = true) { m_grid.setGridFlag(_flag, _active); };
		void setGridFlags(const Grid::GridFlags& _flags) { m_grid.setGridFlags(_flags); };
		const Grid::GridFlags& getGridFlags(void) const { return m_grid.getGridFlags(); };

		void setGridSnapMode(Grid::GridSnapMode _mode) { m_grid.setGridSnapMode(_mode); };
		Grid::GridSnapMode getGridSnapMode(void) const { return m_grid.getGridSnapMode(); };

		void setGridLineStyle(const OutlineF& _outline) { m_grid.setGridLineStyle(_outline); };
		const OutlineF& getGridLineStyle(void) const { return m_grid.getGridLineStyle(); };

		GraphicsView* getGraphicsView(void) { return m_view; };

		void setConnectionPreviewShape(GraphicsConnectionCfg::ConnectionShape _shape) { m_connectionPreviewShape = _shape; };
		GraphicsConnectionCfg::ConnectionShape getConnectionPreviewShape(void) const { return m_connectionPreviewShape; };

		void setIgnoreEvents(bool _ignore) { m_ignoreEvents = _ignore; };
		bool getIgnoreEvents(void) const { return m_ignoreEvents; };

		QPointF snapToGrid(const QPointF& _pos) const;

		Point2DD snapToGrid(const Point2DD& _pos) const;
		
		void deselectAll(void);

		void moveAllSelectedItems(const Point2DD& _delta);

		void rotateAllSelectedItems(double _relativeAngle);

		void flipAllSelectedItems(Qt::Orientation _flipAxis);

		void itemAboutToBeRemoved(GraphicsItem* _item);

	Q_SIGNALS:
		void selectionChangeFinished(void);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Public: Slots

	public Q_SLOTS:
		void slotSelectionChanged(void);
		void handleSelectionChanged(void);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Protected: Event handling
	protected:
		virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent* _event) override;
		virtual void mouseMoveEvent(QGraphicsSceneMouseEvent* _event) override;
		virtual void mousePressEvent(QGraphicsSceneMouseEvent* _event) override;
		virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent* _event) override;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Protected: Helper

		virtual void drawBackground(QPainter* _painter, const QRectF& _rect) override;

		void drawGrid(QPainter* _painter, const QRectF& _rect);

		qreal calculateScaledGridLineWidth(QPainter* _painter, qreal _normalWidth) const;

		void calculateGridLines(const QRectF& _painterRect, QList<QLineF>& _normalLines, QList<QLineF>& _wideLines, QList<QLineF>& _centerLines) const;

		Point2D calculateScaledGridStepSize(const QRectF& _rect) const;

	private:
		bool m_ignoreEvents;
		Grid m_grid;
		GraphicsView* m_view;
		GraphicsItem* m_connectionOrigin;
		GraphicsConnectionPreviewItem* m_connectionPreview;
		ot::GraphicsConnectionCfg::ConnectionShape m_connectionPreviewShape;
		QList<QGraphicsItem*> m_lastSelection;
		bool m_mouseIsPressed;

		GraphicsScene() = delete;
	};

}
