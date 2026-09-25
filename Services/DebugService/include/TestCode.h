// @otlicense

#pragma once

// OpenTwin header
#include "Application.h"

class TestCode
{
	OT_DECL_NOCOPY(TestCode)
	OT_DECL_NOMOVE(TestCode)
	OT_DECL_NODEFAULT(TestCode)
public:
	TestCode(Application* _app) : m_app(_app) {};

	void runTestCode();

private:
	Application* m_app;

};