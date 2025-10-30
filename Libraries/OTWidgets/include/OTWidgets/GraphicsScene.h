// @otlicense
// File: GraphicsScene.h
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
#include "OTSystem/Flags.h"
#include "OTGui/Grid.h"
#include "OTGui/Margins.h"
#include "OTGui/GraphicsConnectionCfg.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtWidgets/qgraphicsscene.h>

// std header
#include <list>

namespace ot {

	class GraphicsView;
	class GraphicsItem;
	class GraphicsElement;
	class GraphicsConnectionItem;
	class GraphicsConnectionPreviewItem;

	//! @brief Graphics Scene for ot::GraphicsItem
	//! Adding QGraphicsItems to the scene that do not inherit ot::GraphicsItem might lead to undefined behavior
	class OT_WIDGETS_API_EXPORT GraphicsScene : public QGraphicsScene {
		Q_OBJECT
		OT_DECL_NOCOPY(GraphicsScene)
		OT_DECL_NOMOVE(GraphicsScene)
	public:
		GraphicsScene(GraphicsView* _view);
		GraphicsScene(const QRectF& _sceneRect, GraphicsView* _view);
		virtual ~GraphicsScene();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Connection handling

		//! @brief A connection line according to the current configuration will be drawn with the provided item as origin
		//! @param _item Origin item
		void startConnection(ot::GraphicsItem* _item);

		//! @brief Will request a connection from the currently set origin to the provided target connection.
		//! The new control point is pos and should be the mouse press position.
		//! @param _targetConnection The destination for the connection request.
		//! @param _pos The new control point position.
		void startConnectionToConnection(ot::GraphicsConnectionItem* _targetedConnection, const Point2DD& _pos);

		void stopConnection(void);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setGrid(const Grid& _grid) { m_grid = _grid; };
		Grid& getGrid(void) { return m_grid; };
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

		void setGridLineStyle(const PenFCfg& _outline) { m_grid.setGridLineStyle(_outline); };
		const PenFCfg& getGridLineStyle(void) const { return m_grid.getGridLineStyle(); };
		
		void setMultiselectionEnabled(bool _enabled) { m_multiselectionEnabled = _enabled; };
		bool getMultiselectionEnabled(void) const { return m_multiselectionEnabled; };

		void checkMaxTriggerDistance(double _triggerDistance);
		void checkMaxTriggerDistance(const MarginsD& _triggerDistance);

		GraphicsView* getGraphicsView(void) { return m_view; };
		const GraphicsView* getGraphicsView(void) const { return m_view; };

		void setConnectionPreviewShape(GraphicsConnectionCfg::ConnectionShape _shape) { m_connectionPreviewShape = _shape; };
		GraphicsConnectionCfg::ConnectionShape getConnectionPreviewShape(void) const { return m_connectionPreviewShape; };

		void setIgnoreEvents(bool _ignore) { m_ignoreEvents = _ignore; };
		bool getIgnoreEvents(void) const { return m_ignoreEvents; };

		QPointF snapToGrid(const QPointF& _pt) const;

		//! @brief Snaps the item to the grid according to the item flags and returns the top left pos.
		QPointF snapToGrid(const GraphicsItem* _item) const;

		void deselectAll(void);

		void moveAllSelectedItems(const Point2DD& _delta);

		void rotateAllSelectedItems(double _relativeAngle);

		void flipAllSelectedItems(Qt::Orientation _flipAxis);

		void itemAboutToBeRemoved(GraphicsItem* _item);

		void connectionAboutToBeRemoved(GraphicsConnectionItem* _connection);

		void elementAboutToBeRemoved(GraphicsElement* _element);

		void handleSelectionChanged(void);

	Q_SIGNALS:
		void selectionChangeFinished(void);
		void graphicsItemDoubleClicked(ot::GraphicsItem* _item);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Public: Slots

	public Q_SLOTS:
		void slotSelectionChanged(void);

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

		GraphicsElement* findClosestConnectableElement(const QPointF& _pos) const;

		QList<QGraphicsItem*> findItemsInTriggerDistance(const QPointF& _pos) const;

	private:
		void handleMultiSelectionChanged(void);
		void handleSingleSelectionChanged(void);


		std::list<GraphicsElement *> m_lastHoverElements;

		bool m_ignoreEvents;
		double m_maxTriggerDistance;
		bool m_multiselectionEnabled;
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
