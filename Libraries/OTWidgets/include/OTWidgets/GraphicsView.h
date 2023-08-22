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
		
		GraphicsItem* getItem(ot::UID _itemUid);
		GraphicsConnectionItem* getConnection(ot::UID _connectionUid);

		void setDropsEnabled(bool _enabled) { m_dropEnabled = _enabled; };

		void setGraphicsViewName(const std::string& _name) { m_viewName = _name; };
		const std::string& graphcisViewName(void) const { return m_viewName; };

		void addConnection(GraphicsItem* _origin, GraphicsItem* _dest);

	signals:
		void itemAdded(ot::UID _uid);
		void connectionAdded(ot::UID _uid);

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

	protected:
		virtual void mousePressedMoveEvent(QMouseEvent* _event) {};

	private:
		std::string m_viewName;
		GraphicsScene* m_scene;
		bool m_isPressed;
		bool m_wheelEnabled;
		bool m_dropEnabled;
		ot::UID m_currentUid;

		std::map<ot::UID, ot::GraphicsItem*> m_items;
		std::map<ot::UID, ot::GraphicsConnectionItem*> m_connections;
	};

}