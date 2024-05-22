//! @file GraphicsView.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTWidgets/OTWidgetsAPIExport.h"
#include "OTCore/CoreTypes.h"

// Qt header
#include <QtWidgets/qgraphicsview.h>
#include "OTGui/GraphicsPackage.h"
#include "OTGui/GraphicsConnectionCfg.h"

// std header
#include <map>
#include <list>
#include <string>
#include <atomic>

namespace ot {

	class GraphicsItem;
	class GraphicsConnectionItem;
	class GraphicsScene;

	//! @brief View widget used to display GraphicsItems
	//! Note that the View creates its own scene.
	class OT_WIDGETS_API_EXPORT GraphicsView : public QGraphicsView {
		Q_OBJECT
	public:
		GraphicsView(GraphicsScene* _scene = (GraphicsScene*)nullptr);
		virtual ~GraphicsView();

		void resetView(void);
		void fitInCurrentView(void);
		void viewAll(void);

		void setMouseWheelEnabled(bool _enabled) { m_wheelEnabled = _enabled; };
		bool mouseWheelEnabled(void) const { return m_wheelEnabled; };

		const bool getStateChangeInProgress() const { return m_stateChangeInProgress; }

		void setGraphicsScene(GraphicsScene* _scene);
		GraphicsScene* getGraphicsScene(void) { return m_scene; };
		
		GraphicsItem* getItem(const ot::UID& _itemUid);
		GraphicsConnectionItem* getConnection(const ot::UID& _connectionUid);
		
		bool connectionAlreadyExists(const ot::GraphicsConnectionCfg& connection);

		void setDropsEnabled(bool _enabled) { m_dropEnabled = _enabled; };

		void setGraphicsViewName(const std::string& _name) { m_viewName = _name; };
		const std::string& graphicsViewName(void) const { return m_viewName; };

		void addItem(ot::GraphicsItem* _item);
		void removeItem(const ot::UID& _itemUid, bool bufferConnections = false);
		std::list<ot::UID> selectedItems(void) const;

		bool addConnectionIfConnectedItemsExist(const GraphicsConnectionCfg& _config);

		void removeConnection(const GraphicsConnectionCfg& _connectionInformation);
		void removeConnection(const ot::UID& _connectionInformation);
		ot::UIDList selectedConnections(void) const;

		void requestConnection(const ot::UID& _fromUid, const std::string& _fromConnector, const ot::UID& _toUid, const std::string& _toConnector);
		void notifyItemMoved(ot::GraphicsItem* _item);

	Q_SIGNALS:
		//! @brief Will be emitted when an item was dropped into the scene by the user
		//! @param _name Item name
		//! @param _pos Item scene position
		void itemRequested(const QString& _name, const QPointF& _pos);

		//! @brief Will be emitted when a connection was "dropped" by the user
		//! @param _fromUid Source item UID
		//! @param _fromConnector Source connector (child of source item)
		//! @param _toUid Destination item UID
		//! @param _toConnector Destination connector (child of destination item)
		void connectionRequested(const ot::UID& _fromUid, const std::string& _fromConnector, const ot::UID& _toUid, const std::string& _toConnector);
		
		void itemMoved(const ot::UID& _uid, const QPointF& _newPos);

		void removeItemsRequested(const ot::UIDList& _items, const ot::UIDList& _connections);

	protected:
		virtual void wheelEvent(QWheelEvent* _event) override;
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
		std::atomic_bool m_stateChangeInProgress = false;
		
		std::string m_viewName;
		GraphicsScene* m_scene;
		bool m_isPressed;
		bool m_wheelEnabled;
		bool m_dropEnabled;
		QPoint m_lastPanPos;

		std::map<ot::UID, ot::GraphicsItem*> m_items;
		std::map<ot::UID, ot::GraphicsConnectionItem*> m_connections;
		std::list<GraphicsConnectionCfg> m_connectionCreationBuffer;
		std::list<GraphicsConnectionCfg> m_itemRemovalConnectionBuffer;

		void addConnection(const GraphicsConnectionCfg& _config);
		bool connectedGraphicItemsExist(const GraphicsConnectionCfg& _config);
	};
}
