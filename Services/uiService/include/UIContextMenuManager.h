// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/BasicEntityInformation.h"
#include "OTGui/Menu/MenuCfg.h"
#include "OTGui/Event/ContextRequestData.h"
#include "OTWidgets/ContextMenu/ContextMenuManager.h"
#include "OTWidgets/ContextMenu/ContextMenuManagerHandler.h"

// Qt header
#include <QtCore/qobject.h>

class AppBase;

namespace ak {
	class aTreeWidget;
}
namespace ot {
	class ContextMenuRequestEvent;
	class ContextMenuCallbackBase;
	class ContextRequestWidgetEvent;
}

class UIContextMenuManager : public ot::ContextMenuManager, public ot::ContextMenuManagerHandler
{
	Q_OBJECT
private:
	AppBase* m_app;

public:
	UIContextMenuManager();
	virtual ~UIContextMenuManager();

	void initialize(AppBase* _app);

	virtual ot::BasicEntityInformation getEntityInformationFromName(const std::string& _entityName, bool* _failFlag = (bool*)nullptr) const override;

protected:
	virtual ot::MenuCfg getContextMenuConfiguration(const ot::ContextMenuCallbackBase* _callbackObject, const ot::ContextMenuRequestEvent& _requestEvent) const override;
	virtual bool notifyContextOwner(const ot::ContextMenuAction* _action) const override;
	virtual bool triggerToolbarButton(const std::string& _buttonName) const override;
};