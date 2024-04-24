#include "stdafx.h"
#include "Model.h"
#include "UiNotifier.h"
#include "Application.h"

#include "OTCore/Logger.h"

void UiNotifier::shortcutActivated(const std::string& _keySquence) {
	Model* model = Application::instance()->getModel();
	if (model) {
		model->keySequenceActivated(_keySquence);
	}
	else {
		OT_LOG_E("No model created yet");
	}
}

void UiNotifier::contextMenuItemClicked(const std::string& _menuName, const std::string& _itemName) {

}

void UiNotifier::contextMenuItemCheckedChanged(const std::string& _menuName, const std::string& _itemName, bool _isChecked) {

}