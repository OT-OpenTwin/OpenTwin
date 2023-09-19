//! @file GraphicsView.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"
#include "OpenTwinCore/CoreTypes.h"

// Qt header
#include <QtWidgets/qgraphicsview.h>

// std header
#include <map>

namespace ot {

	class GraphicsItem;
	class GraphicsConnectionItem;
	class GraphicsScene;

	//! @brief View widget used to display GraphicsItems
	//! Note that the View creates its own scene.
	class OT_WIDGETS_API_EXPORT GraphicsView : public QGraphicsView {
		Q_OBJECT
	public:
		GraphicsView();
		virtual ~GraphicsView();

		void resetView(void);
		void viewAll(void);

		void setMouseWheelEnabled(bool _enabled) { m_wheelEnabled = _enabled; };
		bool mouseWheelEnabled(void) const { return m_wheelEnabled; };

		GraphicsScene* getGraphicsScene(void) { return m_scene; };
		
		GraphicsItem* getItem(const std::string& _itemUid);
		GraphicsConnectionItem* getConnection(const std::string& _connectionUid);

		void setDropsEnabled(bool _enabled) { m_dropEnabled = _enabled; };

		void setGraphicsViewName(const std::string& _name) { m_viewName = _name; };
		const std::string& graphicsViewName(void) const { return m_viewName; };

		void addItem(ot::GraphicsItem* _item);
		void addConnection(GraphicsItem* _origin, GraphicsItem* _dest);

	signals:
		//! @brief Will be emitted when an item was created by the user
		//! The provided item is the item that was created
		void itemCreated(ot::GraphicsItem * _item);
		void connectionCreated(ot::GraphicsItem* _item);

	protected:
		virtual void wheelEvent(QWheelEvent* _event) override;
		virtual void enterEvent(QEvent* _event) override;
		virtual void mousePressEvent(QMouseEvent* _event) override;
		virtual void mouseReleaseEvent(QMouseEvent* _event) override;
		virtual void mouseMoveEvent(QMouseEvent* _event) override;
		virtual void keyPressEvent(QKeyEvent* _event) override;
		virtual void keyReleaseEvent(QKeyEvent* _event) override;
		virtual void resizeEvent(QResizeEvent* _event) override;

		virtual void dragEnterEvent(QDragEnterEvent* _event) override;
		virtual void dropEvent(QDropEvent* _event) override;
		virtual void dragMoveEvent(QDragMoveEvent* _event) override;

	private:
		std::string m_viewName;
		GraphicsScene* m_scene;
		bool m_isPressed;
		bool m_wheelEnabled;
		bool m_dropEnabled;
		QPoint m_lastPanPos;

		std::map<std::string, ot::GraphicsItem*> m_items;
		std::map<std::string, ot::GraphicsConnectionItem*> m_connections;
	};

}