#include "stdafx.h"

#include "Notifier.h"

extern ViewerAPI::Notifier *globalNotifier;

ViewerAPI::Notifier *getNotifier(void)
{
	return globalNotifier;
}
