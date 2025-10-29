// @otlicense

#pragma once
#include "OTCore/OwnerServiceGlobal.h"

ot::OwnerServiceGlobal& ot::OwnerServiceGlobal::instance(void) {
	static OwnerServiceGlobal g_instance;
	return g_instance;
}
