// @otlicense

//! @file ContextMenuManager.h
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/MenuCfg.h"
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtCore/qobject.h>

class QWidget;

namespace ot {

	class ContextMenu;

	class OT_WIDGETS_API_EXPORT ContextMenuManager : public QObject {
		Q_OBJECT
		OT_DECL_NOCOPY(ContextMenuManager)
		OT_DECL_NODEFAULT(ContextMenuManager)
	public:
		ContextMenuManager(QWidget* _widget);
		virtual ~ContextMenuManager();

		void setMenu(const MenuCfg& _config);

	Q_SIGNALS:
		void clearRequested(void);
		void actionTriggered(const std::string& _actionName);

	public Q_SLOTS:
		void slotShowContextMenu(const QPoint& _pos);

	private Q_SLOTS:
		void slotContextActionTriggered(const std::string& _actionName);

	private:
		QWidget* m_widget;
		MenuCfg m_config;
		ContextMenu* m_menu;
	};

}