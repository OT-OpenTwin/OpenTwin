#pragma once

#include <OpenTwinFoundation/AbstractModelNotifier.h>

#include <string>

class ModelNotifier : public ot::AbstractModelNotifier {
public:
	ModelNotifier() {}
	virtual ~ModelNotifier() {}

private:
	ModelNotifier(ModelNotifier&) = delete;
	ModelNotifier& operator = (ModelNotifier&) = delete;
};