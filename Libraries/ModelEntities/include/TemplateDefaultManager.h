// @otlicense
// File: TemplateDefaultManager.h
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#pragma once
#pragma warning(disable : 4251)

#include <string>
#include <map>

namespace ot { class Painter2D; };

class __declspec(dllexport) DefaultValue
{
public:
	DefaultValue();
	DefaultValue(const DefaultValue&other);
	virtual ~DefaultValue();

	DefaultValue& operator=(const DefaultValue&other);

	enum {UNDEFINED, DOUBLE, LONG, BOOL, STRING, COLOR, GUIPainter} type;

	double		d;
	long		l;
	bool		b;
	std::string s;
	int			c[3];
	ot::Painter2D* m_painter;
};

class __declspec(dllexport) TemplateDefaultManager
{
public:
	TemplateDefaultManager();
	virtual ~TemplateDefaultManager();

	void loadDefaultTemplate(void);
	void setDefaultTemplate(const std::string &templateName);
	void loadDefaults(const std::string &category);
	std::string loadDefaultMaterials(void);

	double		getDefaultDouble(const std::string &category, const std::string &value, double defVal);
	long		getDefaultLong(const std::string &category, const std::string &value, long defVal);
	bool		getDefaultBool(const std::string &category, const std::string &value, bool defVal);
	std::string getDefaultString(const std::string &category, const std::string &value, const std::string &defVal);
	int			getDefaultColor(const std::string &category, const std::string &value, int component, int defVal);

	//! \brief Returns the default gui painter for the given category and value.
	//! TemplateDefaultManager keeps ownership of the painter.
	const ot::Painter2D* getDefaultGuiPainter(const std::string& category, const std::string& value);

	static TemplateDefaultManager* getTemplateDefaultManager(void);

	bool isUIMenuPageVisible(const std::string &page);
	bool isUIMenuGroupVisible(const std::string &page, const std::string &group);
	bool isUIMenuSubGroupVisible(const std::string &page, const std::string &group, const std::string& _subgroup);
	bool isUIMenuActionVisible(const std::string &page, const std::string &group, const std::string &action);
	bool isUIMenuActionVisible(const std::string &page, const std::string &group, const std::string& _subgroup, const std::string &action);

	const std::map < std::string, std::map<std::string, DefaultValue>> &getDefaultMaterials(void) { return defaultMaterialsMap; }

private:
	void		 clearSettings(void);
	DefaultValue getDefaultValue(const std::string &category, const std::string &value);

	std::string defaultTemplateName;
	std::map < std::string, std::map<std::string, DefaultValue>> defaultMap;
	std::map < std::string, std::map<std::string, DefaultValue>> defaultMaterialsMap;
};
