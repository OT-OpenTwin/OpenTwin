//! @file GraphicsView.h
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Flags.h"
#include "OTCore/Point2D.h"
#include "OTCore/CoreTypes.h"
#include "OTCore/BasicServiceInformation.h"
#include "OTGui/GraphicsCopyInformation.h"
#include "OTWidgets/QWidgetInterface.h"
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
	class OT_WIDGETS_API_EXPORT GraphicsView : public QGraphicsView, public QWidgetInterface {
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

		virtual QWidget* getQWidget(void) override { return this; };
		virtual const QWidget* getQWidget(void) const override { return this; };

		void resetView(void);
		void ensureViewInBounds(void);

		void setMouseWheelEnabled(bool _enabled) { m_wheelEnabled = _enabled; };
		bool getMouseWheelEnabled(void) const { return m_wheelEnabled; };

		void setOwner(const BasicServiceInformation& _owner) { m_owner = _owner; };
		const BasicServiceInformation& getOwner(void) const { return m_owner; };

		void setGraphicsViewFlag(GraphicsViewFlag _flag, bool _active = true) { m_viewFlags.setFlag(_flag, _active); };
		void setGraphicsViewFlags(const GraphicsViewFlags& _flags) { m_viewFlags = _flags; };
		const GraphicsViewFlags& getGraphicsViewFlags(void) const { return m_viewFlags; };

		void setGraphicsScene(GraphicsScene* _scene);
		GraphicsScene* getGraphicsScene(void) { return m_scene; };
		
		GraphicsItem* getItem(const ot::UID& _itemUid);
		GraphicsConnectionItem* getConnection(const ot::UID& _connectionUid);
		
		bool connectionAlreadyExists(const ot::GraphicsConnectionCfg& connection);

		void setDropsEnabled(bool _enabled) { m_dropEnabled = _enabled; };

		void setGraphicsViewName(const std::string& _name) { m_viewName = _name; };
		const std::string& getGraphicsViewName(void) const { return m_viewName; };

		void addItem(ot::GraphicsItem* _item);
		void removeItem(const ot::UID& _itemUid, bool bufferConnections = false);
		std::list<ot::UID> getSelectedItemUIDs(void) const;
		std::list<GraphicsItem*> getSelectedGraphicsItems(void) const;

		void setSceneMargins(const QMarginsF& _margins) { m_sceneMargins = _margins; };
		const QMarginsF& getSceneMargins(void) const { return m_sceneMargins; };

		bool addConnectionIfConnectedItemsExist(const GraphicsConnectionCfg& _config);

		void removeConnection(const GraphicsConnectionCfg& _connectionInformation);
		void removeConnection(const ot::UID& _connectionInformation);
		ot::UIDList getSelectedConnectionUIDs(void) const;
		std::list<GraphicsConnectionItem*> getSelectedConnectionItems(void) const;

		void requestConnection(const ot::UID& _fromUid, const std::string& _fromConnector, const ot::UID& _toUid, const std::string& _toConnector);

		void requestConnectionToConnection(const ot::UID& _fromItemUid, const std::string& _fromItemConnector, const ot::UID& _toConnectionUid, const ot::Point2DD& _newControlPoint);

		//! @brief Returns true if any item is selected and the itemCopyRequested signal was emitted.
		bool requestCopyCurrentSelection(void);

		//! @brief Returns true if the paste request succeeded and the itemPasteRequested signal was emitted.
		bool requestPasteFromClipboard(void);

		void notifyItemMoved(const ot::GraphicsItem* _item);

		void notifyItemConfigurationChanged(const ot::GraphicsItem* _item);

		QRectF getVisibleSceneRect(void) const;

		void setReadOnly(bool _isReadOnly) { m_viewStateFlags.setFlag(ViewStateFlag::ReadOnlyState, _isReadOnly); };
		bool isReadOnly(void) const { return m_viewStateFlags.flagIsSet(ViewStateFlag::ReadOnlyState); };

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

		//! @brief Is emitted when 
		//! @param _info 
		void itemCopyRequested(GraphicsCopyInformation* _info);

		void itemPasteRequested(GraphicsCopyInformation* _info);

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

	private Q_SLOTS:
		void slotCopy(void);
		void slotPaste(void);

	private:
		//! \brief Begins the item move handling if needed.
		//! If the ot::GraphicsView::ViewStateFlag::ItemMoveInProgress is set the function instantly returns.
		//! All currently selected items will update their move start point.
		void beginItemMove(void);

		//! \brief Ends the item move handling if needed.
		//! If the ot::GraphicsView::ViewStateFlag::ItemMoveInProgress is not set the function instantly returns.
		//! All currently selected items will notify a move change and configuration change if their position has changed.
		void endItemMove(void);

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

		void addConnection(const GraphicsConnectionCfg& _config);
		bool connectedGraphicItemsExist(const GraphicsConnectionCfg& _config);
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::GraphicsView::ViewStateFlag)
OT_ADD_FLAG_FUNCTIONS(ot::GraphicsView::GraphicsViewFlag)