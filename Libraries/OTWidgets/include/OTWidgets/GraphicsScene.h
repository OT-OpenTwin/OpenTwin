//! @file GraphicsScene.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Flags.h"
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
		//! \enum GridFlag
		enum GridFlag {
			NoGridFlags = 0x0000, //! \brief No grid flags.
			ShowNormalLines = 0x0001, //! \brief Basic lines should be drawn.
			ShowWideLines = 0x0002, //! \brief Wide lines should be drawn.
			ShowCenterCross = 0x0004, //! \brief Center cross should be drawn.
			NoGridLineMask = 0xFFF0, //! \brief Mask used to check if no grid line flags are set.

			//! \brief Auto scaling is enabled.
			AutoScaleGrid = 0x0010
		};
		//! \typedef GridFlags
		typedef Flags<GridFlag> GridFlags;

		GraphicsScene(GraphicsView* _view);
		GraphicsScene(const QRectF& _sceneRect, GraphicsView* _view);
		virtual ~GraphicsScene();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Connection handling

		//! @brief A connection line according to the current configuration will be drawn with the provided item as origin
		//! @param _item Origin item
		void startConnection(ot::GraphicsItem* _item);
		void stopConnection(void);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setDefaultGridPen(const QPen& _pen) { m_defaultGridPen = _pen; };
		const QPen& getDefaultGridPen(void) const { return m_defaultGridPen; };

		//! @brief Set the grid size
		//! If the size is set to 0 or less, no grid will be drawn
		//! @param _size The grid size to set
		void setGridStepSize(int _size) { m_gridStepSize = _size; };

		//! @brief Returns the currently set grid size
		int getGridStepSize(void) const { return m_gridStepSize; };

		void setGridWideLineEvery(int _count) { m_gridWideEvery = _count; };
		int getGridWideLineEvery(void) const { return m_gridWideEvery; }

		void setGridFlag(GridFlag _flag, bool _active = true) { m_gridFlags.setFlag(_flag, _active); };
		void setGridFlags(const GridFlags& _flags) { m_gridFlags = _flags; };
		const GridFlags& getGridFlags(void) const { return m_gridFlags; };

		void setGridSnapEnabled(bool _enabled) { m_gridSnapEnabled = _enabled; };
		bool getGridSnapEnabled(void) const { return m_gridSnapEnabled; };

		GraphicsView* getGraphicsView(void) { return m_view; };

		void setConnectionPreviewStyle(GraphicsConnectionCfg::ConnectionStyle _style) { m_connectionPreviewStyle = _style; };
		GraphicsConnectionCfg::ConnectionStyle connectionPreviewStyle(void) const { return m_connectionPreviewStyle; };

		void setIgnoreEvents(bool _ignore) { m_ignoreEvents = _ignore; };
		bool ignoreEvents(void) const { return m_ignoreEvents; };

		QPointF snapToGrid(const QPointF& _pos) const;
		QPointF snapToGrid(const QPointF& _pos, int _gridStepSize) const;

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

		qreal calculateScaledGridLineWidth(QPainter* _painter) const;

		void calculateGridLines(const QRectF& _painterRect, QList<QLineF>& _normalLines, QList<QLineF>& _wideLines, QList<QLineF>& _centerLines) const;

		qreal calculateScaledGridStepSize(const QRectF& _rect) const;

	private:
		QPen m_defaultGridPen;
		bool m_ignoreEvents;
		int m_gridStepSize;
		int m_gridWideEvery;
		bool m_gridSnapEnabled;
		GridFlags m_gridFlags;
		GraphicsView* m_view;
		GraphicsItem* m_connectionOrigin;
		GraphicsConnectionPreviewItem* m_connectionPreview;
		ot::GraphicsConnectionCfg::ConnectionStyle m_connectionPreviewStyle;
		QList<QGraphicsItem*> m_lastSelection;
		bool m_mouseIsPressed;

		GraphicsScene() = delete;
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::GraphicsScene::GridFlag)