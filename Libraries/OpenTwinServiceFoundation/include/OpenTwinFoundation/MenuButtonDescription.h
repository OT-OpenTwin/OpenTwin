#pragma once
#pragma warning(disable : 4251)
#include <string>


namespace ot {
	class __declspec(dllexport)  MenuButtonDescription
	{
	public:
		MenuButtonDescription(const std::string& pageName, const std::string& groupName, const std::string& buttonName)
			: _pageName(pageName), _groupName(groupName), _buttonName(buttonName)
		{};
		MenuButtonDescription(const std::string& pageName, const std::string& groupName, const std::string& subgroupName, const std::string& buttonName)
			: _pageName(pageName), _groupName(groupName), _buttonName(buttonName), _subgroupName(subgroupName)
		{};
		MenuButtonDescription() {};

		void SetDescription(const std::string& pageName, const std::string& groupName, const std::string& buttonName, const std::string& buttonText ="", const std::string& subgroupName = "")
		{
			_pageName = pageName;
			_groupName = groupName;
			_buttonName = buttonName;
			_subgroupName = subgroupName;
			if (buttonText == "")
			{
				_buttonText = buttonName;
			}
		}

		const std::string GetPageName(void) { return _pageName; }
		const std::string GetGroupName(void) { return _groupName; }
		const std::string GetSubgroupName(void) { return _subgroupName; }
		const std::string GetButtonName(void) { return _buttonName; }
		const std::string GetButtonText(void) { return _buttonText; }

		const std::string GetFullDescription()
		{
			if (_subgroupName == "")
			{
				return _pageName + ":" + _groupName + ":" + _buttonName;
			}
			else
			{
				return _pageName + ":" + _groupName + ":" + _subgroupName + ":" + _buttonName;
			}
		}
	private:
		std::string _pageName = "";
		std::string _groupName = "";
		std::string _subgroupName = "";
		std::string _buttonName = "";
		std::string _buttonText = "";
	};
}