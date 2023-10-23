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
#include "OTGui/GraphicsPackage.h"

// std header
#include <map>
#include <string>

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
		void bufferConnection(const ot::GraphicsConnectionPackage::ConnectionInfo& connection);
		void tryAddingBufferedConnections();
		bool connectionAlreadyExists(const ot::GraphicsConnectionPackage::ConnectionInfo& connection);

		void setDropsEnabled(bool _enabled) { m_dropEnabled = _enabled; };

		void setGraphicsViewName(const std::string& _name) { m_viewName = _name; };
		const std::string& graphicsViewName(void) const { return m_viewName; };

		void addItem(ot::GraphicsItem* _item);
		void removeItem(const std::string& _itemUid);
		void addConnection(GraphicsItem* _origin, GraphicsItem* _dest);
		void removeConnection(const std::string& _fromUid, const std::string& _fromConnector, const std::string& _toUid, const std::string& _toConnector);

		void requestConnection(const std::string& _fromUid, const std::string& _fromConnector, const std::string& _toUid, const std::string& _toConnector);
		void notifyItemMoved(ot::GraphicsItem* _item);

	signals:
		//! @brief Will be emitted when an item was dropped into the scene by the user
		//! @param _name Item name
		//! @param _pos Item scene position
		void itemRequested(const QString& _name, const QPointF& _pos);

		//! @brief Will be emitted when a connection was "dropped" by the user
		//! @param _fromUid Source item UID
		//! @param _fromConnector Source connector (child of source item)
		//! @param _toUid Destination item UID
		//! @param _toConnector Destination connector (child of destination item)
		void connectionRequested(const std::string& _fromUid, const std::string& _fromConnector, const std::string& _toUid, const std::string& _toConnector);
		

		void itemMoved(const std::string& _uid, const QPointF& _newPos);

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
		std::list<ot::GraphicsConnectionPackage::ConnectionInfo> m_bufferedConnections;
	};
}