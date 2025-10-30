// @otlicense
// File: DataSourceItemFactory.cpp
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

#include "DataSourceItemFactory.h"

#include "DataSourceManagerItem.h"
#include "EntityBase.h"

#include "DataSourceCartesianVector.h"
#include "DataSourceResult3D.h"
#include "DataSourceUnstructuredMesh.h"
#include "OTCore/LogDispatcher.h"

DataSourceManagerItem *DataSourceItemFactory::createSourceItem(EntityBase *resultEntity)
{
	if (resultEntity->getClassName() == "EntityCartesianVector")
	{
		return new DataSourceCartesianVector;
	}
	else if (resultEntity->getClassName() == "EntityResult3DData")
	{
		return new DataSourceResult3D;
	}
	else if (resultEntity->getClassName() == "EntityResultUnstructuredMeshData")
	{
		return new DataSourceUnstructuredMesh;
	}
	else if (resultEntity->getClassName() == "EntityResultUnstructuredMeshVtk")
	{
		return new DataSourceUnstructuredMesh;
	}
	else
	{
		OT_LOG_EAS("Invalid entity type: \"" + resultEntity->getClassName() + "\"");
	}

	return nullptr;
}

