// @otLicense

#pragma once

// OpenTwin header
#include "OTServiceFoundation/AbstractUiNotifier.h"

// C++ header
#include <string>

class UiNotifier : public ot::AbstractUiNotifier {
public:
	UiNotifier() {}
	virtual ~UiNotifier() {}

	//! @brief Will be called when a key sequence was activated in the ui
	//! @param _keySequence The key sequence that was activated
	virtual void shortcutActivated(const std::string& _keySquence) override;

private:
	UiNotifier(UiNotifier&) = delete;
	UiNotifier& operator = (UiNotifier&) = delete;
};