//! \file ContextMenuAction.h
//! \author Alexander Kuester (alexk95)
//! \date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTGui/MenuButtonCfg.h"
#include "OTWidgets/OTWidgetsAPIExport.h"

// Qt header
#include <QtWidgets/qaction.h>

namespace ot {

	class OT_WIDGETS_API_EXPORT ContextMenuAction : public QAction {
		OT_DECL_NOCOPY(ContextMenuAction)
	public:
		ContextMenuAction(QObject* _parent = (QObject*) nullptr);
		ContextMenuAction(const MenuButtonCfg& _config, QObject* _parent = (QObject*) nullptr);
		virtual ~ContextMenuAction();

		void setFromConfiguration(const MenuButtonCfg& _config);

		void setContextMenuActionName(const std::string& _name) { m_name = _name; };
		const std::string& getContextMenuActionName(void) const { return m_name; };

	private:
		std::string m_name;
	};

}