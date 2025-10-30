// @otlicense

#pragma once

// OpenTwin header
#include "OTServiceFoundation/AbstractModelNotifier.h"

// C++ header
#include <string>

class ModelNotifier : public ot::AbstractModelNotifier {
public:
	ModelNotifier() {}
	virtual ~ModelNotifier() {}

private:
	ModelNotifier(ModelNotifier&) = delete;
	ModelNotifier& operator = (ModelNotifier&) = delete;
};