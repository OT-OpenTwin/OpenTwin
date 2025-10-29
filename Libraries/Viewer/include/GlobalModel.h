// @otlicense

#pragma once

#include "Model.h"
#include "OTCore/OTClassHelper.h"

class GlobalModel {
	OT_DECL_STATICONLY(GlobalModel)
public:
	static Model* instance(void);
	static void setInstance(Model* _model);

private:
	static Model*& getInstance(void);

};