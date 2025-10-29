// @otlicense

#pragma once

#include "OTServiceFoundation/AbstractModelNotifier.h"

#include <string>

class ModelNotifier : public ot::AbstractModelNotifier {
public:
	ModelNotifier() {}
	virtual ~ModelNotifier() {}

private:
	ModelNotifier(ModelNotifier&) = delete;
	ModelNotifier& operator = (ModelNotifier&) = delete;
};