#include "stdafx.h"

#include "Application.h"
#include "ProgressReport.h"

#include "OTCore/CoreTypes.h"
#include "OTCore/JSON.h"
#include "OTCore/Flags.h"
#include "OTCommunication/Msg.h"
#include "OTCommunication/ActionTypes.h"
#include "OTCommunication/UiTypes.h"

void ProgressReport::setProgressInformation(std::string message, bool continuous)
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SetProgressVisibility, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MESSAGE, ot::JsonString(message, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_CONTROL_VisibleState, true, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ContinuousState, continuous, doc.GetAllocator());

	std::string response;
	Application::instance()->sendMessage(true, OT_INFO_SERVICE_TYPE_UI, doc, response);
}

void ProgressReport::setProgress(int percentage)
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SetProgressbarValue, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_PERCENT, percentage, doc.GetAllocator());

	std::string response;
	Application::instance()->sendMessage(true, OT_INFO_SERVICE_TYPE_UI, doc, response);
}

void ProgressReport::closeProgressInformation(void)
{
	ot::JsonDocument doc;
	doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_SetProgressVisibility, doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_MESSAGE, ot::JsonString("", doc.GetAllocator()), doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_CONTROL_VisibleState, false, doc.GetAllocator());
	doc.AddMember(OT_ACTION_PARAM_UI_CONTROL_ContinuousState, false, doc.GetAllocator());

	std::string response;
	Application::instance()->sendMessage(true, OT_INFO_SERVICE_TYPE_UI, doc, response);
}

void ProgressReport::setUILock(bool flag, lockType type)
{
	static int count = 0;

	if (flag)
	{
		if (count == 0)
		{
			ot::JsonDocument doc;
			doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_Lock, doc.GetAllocator()), doc.GetAllocator());
			doc.AddMember(OT_ACTION_PARAM_SERVICE_ID, Application::instance()->serviceID(), doc.GetAllocator());
			
			ot::LockTypeFlags f;

			switch (type)
			{
			case ANY_OPERATION:
				f.setFlag(ot::LockAll);
				break;
			case MODEL_CHANGE:
				f.setFlag(ot::LockModelWrite);
				f.setFlag(ot::LockNavigationWrite);
				f.setFlag(ot::LockViewWrite);
				f.setFlag(ot::LockProperties);
				break;
			default:
				assert(0); // Unknown lock type
				f.setFlag(ot::LockAll);
			}

			//f.setFlag(ot::LockAll);
			doc.AddMember(OT_ACTION_PARAM_ElementLockTypes, ot::JsonArray(ot::toStringList(f), doc.GetAllocator()), doc.GetAllocator()); //see: MicroserviceNotifier->addMenuPushButton
			
			std::string response;
			Application::instance()->sendMessage(true, OT_INFO_SERVICE_TYPE_UI, doc, response);
		}

		count++;
	}
	else
	{
		assert(count > 0);
		if (count > 0)
		{
			count--;

			if (count == 0)
			{
				ot::JsonDocument doc;
				doc.AddMember(OT_ACTION_MEMBER, ot::JsonString(OT_ACTION_CMD_UI_Unlock, doc.GetAllocator()), doc.GetAllocator());
				doc.AddMember(OT_ACTION_PARAM_SERVICE_ID, Application::instance()->serviceID(), doc.GetAllocator());

				ot::LockTypeFlags f;

				switch (type)
				{
				case ANY_OPERATION:
					f.setFlag(ot::LockAll);
					break;
				case MODEL_CHANGE:
					f.setFlag(ot::LockModelWrite);
					f.setFlag(ot::LockNavigationWrite);
					f.setFlag(ot::LockViewWrite);
					f.setFlag(ot::LockProperties);
					break;
				default:
					assert(0); // Unknown lock type
					f.setFlag(ot::LockAll);
				}

				//f.setFlag(ot::LockAll);
				doc.AddMember(OT_ACTION_PARAM_ElementLockTypes, ot::JsonArray(ot::toStringList(f), doc.GetAllocator()), doc.GetAllocator()); //see: MicroserviceNotifier->addMenuPushButton

				std::string response;
				Application::instance()->sendMessage(true, OT_INFO_SERVICE_TYPE_UI, doc, response);
			}
		}
	}
}
