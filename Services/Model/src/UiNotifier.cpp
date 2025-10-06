#include "stdafx.h"
#include "Model.h"
#include "UiNotifier.h"
#include "Application.h"

#include "OTCore/LogDispatcher.h"

void UiNotifier::shortcutActivated(const std::string& _keySquence) {
	Model* model = Application::instance()->getModel();
	if (model) {
		model->keySequenceActivated(_keySquence);
	}
	else {
		OT_LOG_E("No model created yet");
	}
}
