// @otlicense

//! @file ContextMenu.h
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/MenuCfg.h"
#include "OTWidgets/WidgetTypes.h"

// Qt header
#include <QtWidgets/qmenu.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT ContextMenu : public QMenu {
		Q_OBJECT
		OT_DECL_NOCOPY(ContextMenu)
	public:
		ContextMenu(QWidget* _parent = (QWidget*) nullptr);
		ContextMenu(const MenuCfg& _config, QWidget* _parent = (QWidget*) nullptr);
		virtual ~ContextMenu();

		void setFromConfiguration(const MenuCfg& _config);

		void setContextMenuName(const std::string& _name) { m_name = _name; };
		const std::string& getContextMenuName(void) const { return m_name; };

	Q_SIGNALS:
		void contextActionTriggered(const std::string& _actionName);

	private Q_SLOTS:
		void slotActionTriggered(void);
		void slotActionTriggered(const std::string& _actionName);

	private:
		std::string m_name;
	};

}