#pragma once

// OpenTwin header
#include "OpenTwinFoundation/FoundationAPIExport.h"
#include "OpenTwinFoundation/UiComponent.h"
#include "OpenTwinFoundation/ModelComponent.h"
#include "OpenTwinCommunication/UiTypes.h"

// std header
#include <list>
#include <string>
#include <map>
#include <Types.h>

namespace ot {

	class OT_SERVICEFOUNDATION_API_EXPORT ModalCommandBase
	{
	public:
		ModalCommandBase(ot::ApplicationBase *app, const std::string &actionMnuID, const std::string &actionBtnID);
		virtual ~ModalCommandBase();

		virtual bool executeAction(const std::string &action, rapidjson::Document &doc) = 0;

		virtual void modelSelectionChanged(std::list< UID> &selectedEntityID) = 0;

	protected:
		virtual std::string initializeAndCreateUI(ot::Flags<ot::ui::lockType> &modelRead, ot::Flags<ot::ui::lockType> &modelWrite) = 0;

		void setupUI(void);
		void addMenuPage(const std::string &menu);
		void addMenuGroup(const std::string &menu, const std::string &group);
		void addMenuSubgroup(const std::string &menu, const std::string &group, const std::string &subgroup);
		void addMenuAction(const std::string &menu, const std::string &group, const std::string &buttonName, const std::string &text, ot::Flags<ot::ui::lockType> &flags, const std::string &iconName, const std::string &iconFolder = std::string("Default"));
		void addMenuAction(const std::string &menu, const std::string &group, const std::string &subgroup, const std::string &buttonName, const std::string &text, ot::Flags<ot::ui::lockType> &flags, const std::string &iconName, const std::string &iconFolder = std::string("Default"));
		void addMenuCheckBox(const std::string &menu, const std::string &group, const std::string &subgroup, const std::string &boxName, const std::string &boxText, bool checked, ot::Flags<ot::ui::lockType> &flags);
		void addMenuLineEdit(const std::string &menu, const std::string &group, const std::string &subgroup, const std::string &editName, const std::string &editText, const std::string &editLabel, ot::Flags<ot::ui::lockType> &flags);
		void setMenuCheckBox(const std::string &menu, const std::string &group, const std::string &subgroup, const std::string &boxName, bool checked);
		void setMenuLineEdit(const std::string &menu, const std::string &group, const std::string &subgroup, const std::string &editName, const std::string &editText, bool error);

	protected:
		ot::ApplicationBase *application;

	private:
		std::string actionMenuID;
		std::string actionButtonID;

		std::map<std::string, bool>    uiMenuMap;
		std::map<std::string, bool>    uiGroupMap;
		std::map<std::string, bool>    uiSubGroupMap;
		std::map<std::string, bool>    uiActionMap;
	};
}