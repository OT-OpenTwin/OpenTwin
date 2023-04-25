#pragma once
#pragma warning(disable : 4251)
#include <string>


namespace ot {
	class __declspec(dllexport)  MenuButtonDescription
	{
	public:
		MenuButtonDescription(std::string pageName, std::string groupName, std::string buttonName)
			: _pageName(pageName), _groupName(groupName), _buttonName(buttonName)
		{};
		MenuButtonDescription() {};

		void SetDescription(std::string pageName, std::string groupName, std::string buttonName, std::string buttonText ="")
		{
			_pageName = pageName;
			_groupName = groupName;
			_buttonName = buttonName;
			if (buttonText == "")
			{
				_buttonText = buttonName;
			}
		}

		const std::string GetPageName(void) { return _pageName; }
		const std::string GetGroupName(void) { return _groupName; }
		const std::string GetButtonName(void) { return _buttonName; }
		const std::string GetButtonText(void) { return _buttonText; }

		const std::string GetFullDescription()
		{
			return _pageName + ":" + _groupName + ":" + _buttonName;
		}
	private:
		std::string _pageName;
		std::string _groupName;
		std::string _buttonName;
		std::string _buttonText;
	};
}