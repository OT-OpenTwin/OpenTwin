// @otlicense

#pragma once
#include "Application.h"
#include "MicroserviceNotifier.h"

class QueuingHttpRequestsRAII
{
public:
	QueuingHttpRequestsRAII()
	{
		Application::instance()->getNotifier()->enableQueuingHttpRequests(true);
	}
	~QueuingHttpRequestsRAII()
	{
		Application::instance()->getNotifier()->enableQueuingHttpRequests(false);
	}
};
