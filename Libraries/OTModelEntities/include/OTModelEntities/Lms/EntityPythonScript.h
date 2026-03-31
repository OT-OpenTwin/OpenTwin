// @otlicense
// File: EntityPythonScript.h
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


#include "OTModelEntities/EntityFileText.h"

class OT_MODELENTITIES_API_EXPORT EntityPythonScript : public EntityFileText {

public:
	EntityPythonScript() = default;
	virtual ~EntityPythonScript() = default;

	virtual void setLibraryElement(const ot::LibraryElement& _libraryElement, ot::NewModelStateInfo& _modelState) override;
	static std::string className() { return "EntityPythonScript"; };
	virtual std::string getClassName(void) const override { return EntityPythonScript::className(); };
};