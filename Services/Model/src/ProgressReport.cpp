#include "stdafx.h"

#include "ProgressReport.h"

#include "OpenTwinCore/CoreTypes.h"
#include "OpenTwinCore/rJSON.h"
#include "OpenTwinCore/Flags.h"
#include "OpenTwinCommunication/Msg.h"
#include "OpenTwinCommunication/ActionTypes.h"
#include "OpenTwinCommunication/UiTypes.h"

extern std::string globalUIserviceURL;
extern std::string globalServiceURL;
extern ot::serviceID_t globalServiceID;

void ProgressReport::setProgressInformation(std::string message, bool continuous)
{
	OT_rJSON_createDOC(doc);
	ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_SetProgressVisibility);
	ot::rJSON::add(doc, OT_ACTION_PARAM_MESSAGE, message);
	ot::rJSON::add(doc, OT_ACTION_PARAM_UI_CONTROL_VisibleState, true);
	ot::rJSON::add(doc, OT_ACTION_PARAM_UI_CONTROL_ContinuousState, continuous);

	std::string response;
	ot::msg::send(globalServiceURL, globalUIserviceURL, ot::QUEUE, ot::rJSON::toJSON(doc), response);
}

void ProgressReport::setProgress(int percentage)
{
	OT_rJSON_createDOC(doc);
	ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_SetProgressbarValue);
	ot::rJSON::add(doc, OT_ACTION_PARAM_PERCENT, percentage);

	std::string response;
	ot::msg::send(globalServiceURL, globalUIserviceURL, ot::QUEUE, ot::rJSON::toJSON(doc), response);
}

void ProgressReport::closeProgressInformation(void)
{
	OT_rJSON_createDOC(doc);
	ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_SetProgressVisibility);
	ot::rJSON::add(doc, OT_ACTION_PARAM_MESSAGE, "");
	ot::rJSON::add(doc, OT_ACTION_PARAM_UI_CONTROL_VisibleState, false);
	ot::rJSON::add(doc, OT_ACTION_PARAM_UI_CONTROL_ContinuousState, false);

	std::string response;
	ot::msg::send(globalServiceURL, globalUIserviceURL, ot::QUEUE, ot::rJSON::toJSON(doc), response);
}

void ProgressReport::setUILock(bool flag, lockType type)
{
	static int count = 0;

	if (flag)
	{
		if (count == 0)
		{
			OT_rJSON_createDOC(doc);
			ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_Lock);
			ot::rJSON::add(doc, OT_ACTION_PARAM_SERVICE_ID, globalServiceID);
			
			ot::Flags<ot::ui::lockType> f;

			switch (type)
			{
			case ANY_OPERATION:
				f.setFlag(ot::ui::lockType::tlAll);
				break;
			case MODEL_CHANGE:
				f.setFlag(ot::ui::lockType::tlModelWrite);
				f.setFlag(ot::ui::lockType::tlNavigationWrite);
				f.setFlag(ot::ui::lockType::tlViewWrite);
				f.setFlag(ot::ui::lockType::tlProperties);
				break;
			default:
				assert(0); // Unknown lock type
				f.setFlag(ot::ui::lockType::tlAll);
			}

			//f.setFlag(ot::ui::lockType::tlAll);
			ot::rJSON::add(doc, OT_ACTION_PARAM_ElementLockTypes, ot::ui::toList(f)); //see: MicroserviceNotifier->addMenuPushButton
			
			std::string response;
			ot::msg::send(globalServiceURL, globalUIserviceURL, ot::QUEUE, ot::rJSON::toJSON(doc), response);
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
				OT_rJSON_createDOC(doc);
				ot::rJSON::add(doc, OT_ACTION_MEMBER, OT_ACTION_CMD_UI_Unlock);
				ot::rJSON::add(doc, OT_ACTION_PARAM_SERVICE_ID, globalServiceID);

				ot::Flags<ot::ui::lockType> f;

				switch (type)
				{
				case ANY_OPERATION:
					f.setFlag(ot::ui::lockType::tlAll);
					break;
				case MODEL_CHANGE:
					f.setFlag(ot::ui::lockType::tlModelWrite);
					f.setFlag(ot::ui::lockType::tlNavigationWrite);
					f.setFlag(ot::ui::lockType::tlViewWrite);
					f.setFlag(ot::ui::lockType::tlProperties);
					break;
				default:
					assert(0); // Unknown lock type
					f.setFlag(ot::ui::lockType::tlAll);
				}

				//f.setFlag(ot::ui::lockType::tlAll);
				ot::rJSON::add(doc, OT_ACTION_PARAM_ElementLockTypes, ot::ui::toList(f)); //see: MicroserviceNotifier->addMenuPushButton

				std::string response;
				ot::msg::send(globalServiceURL, globalUIserviceURL, ot::QUEUE, ot::rJSON::toJSON(doc), response);
			}
		}
	}
}
