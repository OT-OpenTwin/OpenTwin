
#pragma once
#include <gtest/gtest.h>
#include "EntityPythonManifest.h"
#include "FileHelper.h"
class FixturePythonManifest : public testing::Test
{
public:
	std::list<std::string> getPackageList(const std::string _text)
	{
		EntityPythonManifest manifest;
		return manifest.getPackageList(_text);
	}
	bool environmentHasChanged(const std::string& _oldContent, const std::string& _newContent){
		EntityPythonManifest manifest;
		manifest.m_manifestText = _oldContent;
		return manifest.environmentHasChanged(_newContent);
	}
	//! @brief Base manifest with multiple packages
	std::string getManifestA()
	{
		const std::string path = "C:\\OpenTwin\\Libraries\\ModelEntities\\ModelEntitiesTest\\Files\\";
		const std::string manifest = FileHelper::extractFileContentAsString(path + "ManifestA.txt");
		return manifest;
	}
	
	//! @brief Derived from ManifestA without the scipy package
	std::string getManifestB()
	{
		const std::string path = FileHelper::getFilePath();
		const std::string manifest = FileHelper::extractFileContentAsString(path + "ManifestB.txt");
		return manifest;
	}

	//! @brief Derived from ManifestA but matplotlib has different version
	std::string getManifestC()
	{
		const std::string path = FileHelper::getFilePath();
		const std::string manifest = FileHelper::extractFileContentAsString(path + "ManifestC.txt");
		return manifest;
	}

	//! @brief Derived from ManifestA but with empty lines tabs and spaces
	std::string getManifestD()
	{
		const std::string path = FileHelper::getFilePath();
		const std::string manifest = FileHelper::extractFileContentAsString(path + "ManifestD.txt");
		return manifest;
	}

	//! @brief Derived from ManifestA but with broken syntax
	std::string getManifestE()
	{
		const std::string path = FileHelper::getFilePath();
		const std::string manifest = FileHelper::extractFileContentAsString(path + "ManifestE.txt");
		return manifest;
	}
};
