// @otlicense
// File: WidgetDebugHelper.cpp
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

// OpenTwin header
#include "OTWidgets/Debugging/WidgetDebugHelper.h"

// std header
#include <sstream>

std::string ot::WidgetDebugHelper::objectHierarchyString(QObject* _object, const std::string& _linePrefix)
{
	std::list<QObject*> hierarchy;
	
	while (_object)
	{
		hierarchy.push_front(_object);
		_object = _object->parent();
	}
	
	int level = 0;
	std::stringstream ss;
	for (QObject* obj : hierarchy)
	{
		ss << _linePrefix;
		for (int i = 0; i < level; i++)
		{
			ss << "  ";
		}
		ss << obj->metaObject()->className() << " (" << obj->objectName().toStdString() << ")" << std::endl;
		level++;
	}

	return ss.str();
}
