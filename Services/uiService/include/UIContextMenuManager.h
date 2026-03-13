// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/BasicEntityInformation.h"
#include "OTGui/Menu/MenuCfg.h"
#include "OTGui/Event/MenuRequestData.h"
#include "OTWidgets/Menu/MenuManagerHandler.h"
#include "OTWidgets/Menu/MenuManager.h"

// Qt header
#include <QtCore/qobject.h>

class AppBase;

namespace ak {
	class aTreeWidget;
}
namespace ot {
	class MenuRequestEvent;
	class MenuCallbackBase;
	class RequestWidgetEvent;
}

class UIContextMenuManager : public ot::MenuManager, public ot::MenuManagerHandler
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
	virtual ot::MenuCfg getMenuConfiguration(const ot::MenuCallbackBase* _callbackObject, const ot::MenuRequestEvent& _requestEvent) const override;
	virtual bool notifyMenuOwner(const ot::MenuAction* _action) const override;
	virtual bool triggerToolbarButton(const std::string& _buttonName) const override;
};