#pragma once
#include <string>
#include <list>
#include "OTSystem/SingleSignOn/GroupInfos.h"
namespace ot
{
	struct LogInInfos
	{
		std::string m_userName;
		std::list<ot::GroupInfo> m_userGroups;
		std::string m_packageName;
	};
}
