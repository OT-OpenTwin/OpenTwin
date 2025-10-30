// @otlicense
// File: IC3DCreator.h
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

#ifndef IC3DCREATOR_H
#define IC3DCREATOR_H

#include "IC.h"
#include "Viewer.h"
#include "ShapeUtilities.h"
#include "STEPWriter.h"
#include <XCAFDoc_ShapeTool.hxx>
#include <XCAFDoc_ColorTool.hxx>

class IC3DCreator
{
public:
	void createIC(IC& ic, STEPWriter& stepWriter, Viewer& viewer);
};

#endif // IC3DCREATOR_H