// @otlicense

// Viewer header
#include "stdafx.h"
#include "FrontendAPI.h"

void FrontendAPI::setInstance(FrontendAPI* _api) {
	FrontendAPI::getInstance() = _api;
}

FrontendAPI* FrontendAPI::instance(void) {
	return FrontendAPI::getInstance();
}

FrontendAPI*& FrontendAPI::getInstance(void) {
	static FrontendAPI* g_instance{ nullptr };
	return g_instance;
}
