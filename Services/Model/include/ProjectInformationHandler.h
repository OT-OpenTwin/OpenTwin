// @otlicense

#pragma once

// OpenTwin header
#include "OTCommunication/ActionHandler.h"
#include "OTGuiAPI/ButtonHandler.h"
#include "OTServiceFoundation/UiComponent.h"

class ProjectInformationHandler : public ot::ActionHandler, public ot::ButtonHandler {
	OT_DECL_NOCOPY(ProjectInformationHandler)
	OT_DECL_NOMOVE(ProjectInformationHandler)
public:
	ProjectInformationHandler(ot::ActionDispatcher* _dispatcher = &ot::ActionDispatcher::instance());
	~ProjectInformationHandler();

	void addButtons(ot::components::UiComponent* _ui);

	// ##################################################################################################################################################################################################################

	// Action and Button handler

private:
	ot::ReturnMessage applyProjectInformation(ot::JsonDocument& _document);
	void requestUploadProjectPreviewImage();

	ot::ToolBarButtonCfg m_editButton;
};