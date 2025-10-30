// @otlicense
// File: VtkDriverFactory.cpp
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

#include "VtkDriverFactory.h"

#include "VtkDriver.h"
#include "EntityVis2D3D.h"

#include "VtkDriverGeneric.h"
#include "VtkDriverCartesianFaceScalar.h"
#include "VtkDriverCartesianVectorfield.h"
#include "VtkDriverUnstructuredScalarSurface.h"
#include "VtkDriverUnstructuredScalarVolume.h"
#include "VtkDriverUnstructuredVectorVolume.h"
#include "VtkDriverUnstructuredVectorSurface.h"

VtkDriver *VtkDriverFactory::createDriver(EntityVis2D3D *visEntity)
{
	if (visEntity->getClassName() == "EntityVisCartesianFaceScalar")
	{
		return new VtkDriverCartesianFaceScalar;
	}
	else if (visEntity->getClassName() == "EntityResult3D") 
	{
		return new VtkDriverCartesianVectorfield;
	}
	else if (visEntity->getClassName() == "EntityVisUnstructuredScalarSurface") 
	{
		return new VtkDriverUnstructuredScalarSurface;
	}
	else if (visEntity->getClassName() == "EntityVisUnstructuredScalarVolume") 
	{
		return new VtkDriverUnstructuredScalarVolume;
	}
	else if (visEntity->getClassName() == "EntityVisUnstructuredVectorVolume") 
	{
		return new VtkDriverUnstructuredVectorVolume;
	}
	else if (visEntity->getClassName() == "EntityVisUnstructuredVectorSurface")
	{
		return new VtkDriverUnstructuredVectorSurface;
	}
	else
	{
		assert(0); // Unknown data type
	}

	return new VtkDriverGeneric;
}

