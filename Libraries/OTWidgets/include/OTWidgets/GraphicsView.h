//! @file GraphicsView.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header	
#include "OTSystem/Flags.h"
#include "OTCore/Point2D.h"
#include "OTCore/CoreTypes.h"
#include "OTCore/BasicServiceInformation.h"
#include "OTGui/CopyInformation.h"
#include "OTWidgets/WidgetBase.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

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
	class GraphicsScene;
	class GraphicsItemCfg;
	class GraphicsConnectionItem;

	//! @brief View widget used to display GraphicsItems
	//! Note that the View creates its own scene.
	class OT_WIDGETS_API_EXPORT GraphicsView : public QGraphicsView, public WidgetBase {
		Q_OBJECT
		OT_DECL_NOCOPY(GraphicsView)
		OT_DECL_NOMOVE(GraphicsView)
	public:
		enum GraphicsViewFlag {
			NoViewFlags            = 0x00, //! @brief No flags.
			ViewManagesSceneRect   = 0x01, //! @brief If set the view manages the scene rect when zooming or panning.
			IgnoreConnectionByUser = 0x02  //! @brief If set the user can not create connections.
		};
		typedef Flags<GraphicsViewFlag> GraphicsViewFlags;

		enum ViewStateFlag {
			DefaultState            = 0 << 0,
			ItemMoveInProgress      = 1 << 0,
			MiddleMousePressedState = 1 << 1,
			ReadOnlyState           = 1 << 2
		};
		typedef ot::Flags<ViewStateFlag> ViewStateFlags;

		GraphicsView(GraphicsScene* _scene = (GraphicsScene*)nullptr);
		virtual ~GraphicsView();

		// ###########################################################################################################################################################################################################################################################################################################################

		// Virtual methods

		virtual QWidget* getQWidget() override { return this; };
		virtual const QWidget* getQWidget() const override { return this; };

		void setMouseWheelEnabled(bool _enabled) { m_wheelEnabled = _enabled; };
		bool getMouseWheelEnabled() const { return m_wheelEnabled; };

		void setOwner(const BasicServiceInformation& _owner) { m_owner = _owner; };
		const BasicServiceInformation& getOwner() const { return m_owner; };

		void setGraphicsViewFlag(GraphicsViewFlag _flag, bool _active = true) { m_viewFlags.setFlag(_flag, _active); };
		void setGraphicsViewFlags(const GraphicsViewFlags& _flags) { m_viewFlags = _flags; };
		const GraphicsViewFlags& getGraphicsViewFlags() const { return m_viewFlags; };

		void setDropsEnabled(bool _enabled) { m_dropEnabled = _enabled; };

		void setGraphicsViewName(const std::string& _name);
		const std::string& getGraphicsViewName() const { return m_viewName; };

		void setReadOnly(bool _isReadOnly) { m_viewStateFlags.setFlag(ViewStateFlag::ReadOnlyState, _isReadOnly); };
		bool isReadOnly() const { return m_viewStateFlags.flagIsSet(ViewStateFlag::ReadOnlyState); };

		void setGraphicsScene(GraphicsScene* _scene);
		GraphicsScene* getGraphicsScene() { return m_scene; };
		const GraphicsScene* getGraphicsScene() const { return m_scene; };

		void setSceneMargins(const QMarginsF& _margins) { m_sceneMargins = _margins; };
		const QMarginsF& getSceneMargins() const { return m_sceneMargins; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// View handling

		void resetView();
		void ensureViewInBounds();

		void setGraphicsSceneRect(double _posX, double _posY, double _sizeX, double _sizeY) { this->setGraphicsSceneRect(QRectF(QPointF(_posX, _posY), QSizeF(_sizeX, _sizeY))); };
		void setGraphicsSceneRect(const QPointF& _pos, const QSize& _size) { this->setGraphicsSceneRect(QRectF(_pos, _size)); };
		void setGraphicsSceneRect(const QRectF& _rect);

		QRectF getVisibleSceneRect() const;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Item handling
		
		GraphicsItem* getItem(const ot::UID& _itemUid);
		GraphicsConnectionItem* getConnection(const ot::UID& _connectionUid);
		
		//! @brief Will rename the item if it exists.
		//! @param _oldEntityName Old item entity name.
		//! @param _newEntityName New item entity name.
		void renameItem(const std::string& _oldEntityName, const std::string& _newEntityName);

		bool connectionAlreadyExists(const ot::GraphicsConnectionCfg& connection);

		//! @brief Adds the given item to the view and scene.
		//! If an item with the same UID already exists it will be removed first.
		//! The item will be owned by the view and will be deleted when removed from the view or when the view is deleted.
		//! Existing connections will connect to the item if the UIDs match.
		//! @param _item Item to add.
		void addItem(ot::GraphicsItem* _item);

		//! @brief Removes the item with the given UID from the view and scene.
		//! The item will be deleted.
		//! The item will also be removed from all connections and the connections will update their configuration.
		//! @param _itemUid Item UID to remove.
		void removeItem(const ot::UID& _itemUid);
		
		//! @brief Selects the item with the given UID.
		//! Items not in the list will be deselected.
		//! During this operation no signals will be emitted.
		//! @param _uids List of item UIDs to select.
		void setSelectedElements(const ot::UIDList& _uids);

		std::list<ot::UID> getSelectedItemUIDs() const;
		std::list<GraphicsItem*> getSelectedGraphicsItems() const;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Connection handling

		//! @brief Adds the given connection to the view and scene.
		//! If a connection with the same UID already exists it will be removed first.
		//! The connection will be owned by the view and will be deleted when removed from the view or when the view is deleted.
		//! Existing items will connect to the connection if the UIDs match.
		//! @param _config Connection configuration to add.
		void addConnection(const GraphicsConnectionCfg& _config);

		void removeConnection(const GraphicsConnectionCfg& _connectionInformation);
		void removeConnection(const ot::UID& _connectionInformation);
		ot::UIDList getSelectedConnectionUIDs() const;
		std::list<GraphicsConnectionItem*> getSelectedConnectionItems() const;

		//! @brief Emits the connectionRequested signal if the connection does not already exist.
		//! @param _fromUid Origin item UID.
		//! @param _fromConnector Origin connector (child of origin item).
		//! @param _toUid Destination item UID.
		//! @param _toConnector Destination connector (child of destination item).
		void requestConnection(const ot::UID& _fromUid, const std::string& _fromConnector, const ot::UID& _toUid, const std::string& _toConnector);

		//! @brief Emits the connectionToConnectionRequested signal.
		//! @param _fromItemUid Origin item UID.
		//! @param _fromItemConnector Origin connector (child of origin item).
		//! @param _toConnectionUid Destination connection UID.
		//! @param _newControlPoint New control point in scene coordinates.
		void requestConnectionToConnection(const ot::UID& _fromItemUid, const std::string& _fromItemConnector, const ot::UID& _toConnectionUid, const ot::Point2DD& _newControlPoint);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Callback handling

		void notifyItemMoved(const ot::GraphicsItem* _item);

		void notifyItemConfigurationChanged(const ot::GraphicsItem* _item);

		void notifyConnectionChanged(const ot::GraphicsConnectionItem* _connection);

	Q_SIGNALS:
		//! @brief Will be emitted when an item was dropped into the scene by the user.
		//! @param _name Item name.
		//! @param _pos Item scene position.
		void itemRequested(const QString& _name, const QPointF& _pos);

		//! @brief Will be emitted when a connection was "dropped" by the user.
		//! @param _fromUid Source item UID.
		//! @param _fromConnector Source connector (child of source item).
		//! @param _toUid Destination item UID.
		//! @param _toConnector Destination connector (child of destination item).
		void connectionRequested(const ot::UID& _fromUid, const std::string& _fromConnector, const ot::UID& _toUid, const std::string& _toConnector);
		
		//! @brief Will be emitted when a connection to a connection was "dropped" by the user.
		//! @param _fromItemUid Origin item UID.
		//! @param _fromItemConnector Origin connector (child of origin item).
		//! @param _toConnectionUid Destination connection UID.
		//! @param _newControlPoint New control point in scene coordinates.
		void connectionToConnectionRequested(const ot::UID& _fromItemUid, const std::string& _fromItemConnector, const ot::UID& _toConnectionUid, const ot::Point2DD& _newControlPoint);

		void itemMoved(const ot::UID& _uid, const QPointF& _newPos);

		void itemConfigurationChanged(const ot::GraphicsItemCfg* _newConfig);

		void itemDoubleClicked(const ot::GraphicsItemCfg* _itemConfig);

		void connectionChanged(const ot::GraphicsConnectionCfg& _newConfig);

		void removeItemsRequested(const ot::UIDList& _items, const ot::UIDList& _connections);

		void copyRequested(CopyInformation& _info);
		  
		void pasteRequested(CopyInformation& _info);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Protected: Events

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

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private: Slots

	private Q_SLOTS:
		void slotCopy();
		void slotPaste();
		void slotGraphicsItemDoubleClicked(ot::GraphicsItem* _item);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private: Helper

	private:
		//! @brief Begins the item move handling if needed.
		//! If the ot::GraphicsView::ViewStateFlag::ItemMoveInProgress is set the function instantly returns.
		//! All currently selected items will update their move start point.
		void beginItemMove();

		//! @brief Ends the item move handling if needed.
		//! If the ot::GraphicsView::ViewStateFlag::ItemMoveInProgress is not set the function instantly returns.
		//! All currently selected items will notify a move change and configuration change if their position has changed.
		void endItemMove();

		BasicServiceInformation m_owner;

		GraphicsViewFlags m_viewFlags;
		ViewStateFlags m_viewStateFlags;

		std::string m_viewName;
		GraphicsScene* m_scene;
		QMarginsF m_sceneMargins;

		bool m_wheelEnabled;
		bool m_dropEnabled;
		
		QPoint m_lastPanPos;
	
		std::map<ot::UID, ot::GraphicsItem*> m_items;
		std::map<ot::UID, ot::GraphicsConnectionItem*> m_connections;
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::GraphicsView::ViewStateFlag)
OT_ADD_FLAG_FUNCTIONS(ot::GraphicsView::GraphicsViewFlag)