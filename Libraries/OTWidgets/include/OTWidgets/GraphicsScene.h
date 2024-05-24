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
		enum GridMode {
			NoGrid,
			BasicGrid,
			BasicGridWithCenter,
			AdvancedGrid,
			AdvancedGridNomalCenter,
			WideLinesOnly
		};

		GraphicsScene(GraphicsView * _view);
		GraphicsScene(const QRectF& _sceneRect, GraphicsView * _view);
		virtual ~GraphicsScene();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Connection handling

		//! @brief A connection line according to the current configuration will be drawn with the provided item as origin
		//! @param _item Origin item
		void startConnection(ot::GraphicsItem* _item);
		void stopConnection(void);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		//! @brief Set the grid size
		//! If the size is set to 0 or less, no grid will be drawn
		//! @param _size The grid size to set
		void setGridStepSize(int _size) { m_gridStepSize = _size; };

		//! @brief Returns the currently set grid size
		int getGridStepSize(void) const { return m_gridStepSize; };

		void setGridWideLineEvery(int _count) { m_gridWideEvery = _count; };
		int getGridWideLineEvery(void) const { return m_gridWideEvery; }

		void setGridMode(GridMode _mode) { m_gridMode = _mode; };
		GridMode getGridMode(void) const { return m_gridMode; };

		GraphicsView* getGraphicsView(void) { return m_view; };

		void setConnectionPreviewStyle(GraphicsConnectionCfg::ConnectionStyle _style) { m_connectionPreviewStyle = _style; };
		GraphicsConnectionCfg::ConnectionStyle connectionPreviewStyle(void) const { return m_connectionPreviewStyle; };

		void setIgnoreEvents(bool _ignore) { m_ignoreEvents = _ignore; };
		bool ignoreEvents(void) const { return m_ignoreEvents; };

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

		qreal calculateScaledLineWidth(QPainter* _painter) const;

		void calculateGridLines(const QRectF& _painterRect, QList<QLineF>& _normalLines, QList<QLineF>& _wideLines, QList<QLineF>& _centerLines) const;

	private:
		qreal m_penWidth;
		bool m_ignoreEvents;
		int m_gridStepSize;
		int m_gridWideEvery;
		GridMode m_gridMode;
		GraphicsView* m_view;
		GraphicsItem* m_connectionOrigin;
		GraphicsConnectionPreviewItem* m_connectionPreview;
		ot::GraphicsConnectionCfg::ConnectionStyle m_connectionPreviewStyle;
		QList<QGraphicsItem*> m_lastSelection;
		bool m_mouseIsPressed;

		GraphicsScene() = delete;
	};

}
