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
	public:
		enum GraphicsViewFlag {
			NoViewFlags            = 0x00, //! \brief No flags.
			ViewManagesSceneRect   = 0x01, //! \brief If set the view manages the scene rect when zooming or panning.
			IgnoreConnectionByUser = 0x02  //! \brief If set the user can not create connections.
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

		void addItem(ot::GraphicsItem* _item);
		void removeItem(const ot::UID& _itemUid, bool bufferConnections = false);
		
		//! @brief Selects the item with the given UID.
		//! Items not in the list will be deselected.
		//! During this operation no signals will be emitted.
		//! @param _uids List of item UIDs to select.
		void setSelectedElements(const ot::UIDList& _uids);

		std::list<ot::UID> getSelectedItemUIDs() const;
		std::list<GraphicsItem*> getSelectedGraphicsItems() const;

		bool addConnectionIfConnectedItemsExist(const GraphicsConnectionCfg& _config);

		void removeConnection(const GraphicsConnectionCfg& _connectionInformation);
		void removeConnection(const ot::UID& _connectionInformation);
		ot::UIDList getSelectedConnectionUIDs() const;
		std::list<GraphicsConnectionItem*> getSelectedConnectionItems() const;

		void requestConnection(const ot::UID& _fromUid, const std::string& _fromConnector, const ot::UID& _toUid, const std::string& _toConnector);

		void requestConnectionToConnection(const ot::UID& _fromItemUid, const std::string& _fromItemConnector, const ot::UID& _toConnectionUid, const ot::Point2DD& _newControlPoint);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Callback handling

		void notifyItemMoved(const ot::GraphicsItem* _item);

		void notifyItemConfigurationChanged(const ot::GraphicsItem* _item);

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
		
		void connectionToConnectionRequested(const ot::UID& _fromItemUid, const std::string& _fromItemConnector, const ot::UID& _toConnectionUid, const ot::Point2DD& _newControlPoint);

		void itemMoved(const ot::UID& _uid, const QPointF& _newPos);

		void itemConfigurationChanged(const ot::GraphicsItemCfg* _newConfig);

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
		void slotCopy(void);
		void slotPaste(void);

		// ###########################################################################################################################################################################################################################################################################################################################

		// Private: Helper

	private:
		//! \brief Begins the item move handling if needed.
		//! If the ot::GraphicsView::ViewStateFlag::ItemMoveInProgress is set the function instantly returns.
		//! All currently selected items will update their move start point.
		void beginItemMove();

		//! \brief Ends the item move handling if needed.
		//! If the ot::GraphicsView::ViewStateFlag::ItemMoveInProgress is not set the function instantly returns.
		//! All currently selected items will notify a move change and configuration change if their position has changed.
		void endItemMove();

		void addConnection(const GraphicsConnectionCfg& _config);
		bool connectedGraphicItemsExist(const GraphicsConnectionCfg& _config);

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
		std::list<GraphicsConnectionCfg> m_connectionCreationBuffer;
		std::list<GraphicsConnectionCfg> m_itemRemovalConnectionBuffer;
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::GraphicsView::ViewStateFlag)
OT_ADD_FLAG_FUNCTIONS(ot::GraphicsView::GraphicsViewFlag)