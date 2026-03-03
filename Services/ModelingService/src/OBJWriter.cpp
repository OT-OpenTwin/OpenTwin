// @otlicense
// File: OBJWriter.cpp
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

#include "OBJWriter.h"
#include "OTCADEntities/EntityGeometry.h"
#include "OTCADEntities/GeometryOperations.h"
#include "Application.h"

#include "OTCommunication/Msg.h"
#include "OTModelEntities/DataBase.h"
#include "OTModelEntities/EntityAPI.h"
#include "OTServiceFoundation/ModelComponent.h"
#include "OTServiceFoundation/uiComponent.h"
#include "OTModelAPI/ModelServiceAPI.h"
#include "OTModelEntities/EntityFacetData.h"
#include "OTModelEntities/Geometry.h"

#include <iostream>
#include <filesystem>

#include "Precision.hxx"

#include "base64.h"
#include "zlib.h"

void OBJWriter::initializeWriter()
{
	offset = 1; // The vertex numbering starts at 1 in obj, but starts at 0 in the entity data structure
}

void OBJWriter::appendData(const std::string &objectName, EntityFacetData* facetEntity, std::stringstream &dataStream)
{
	// Write the object key
	dataStream << "o " << objectName << "\n";

	// Now store all vertices
	for (auto& vertex : facetEntity->getNodeVector())
	{
		dataStream << "v " << vertex.getCoord(0) << " " << vertex.getCoord(1) << " " << vertex.getCoord(2) << std::endl;
	}

	// Store the texture coordinates (per vertex)
	for (auto& vertex : facetEntity->getNodeVector())
	{
		dataStream << "vt " << vertex.getUVpar(0) << " " << vertex.getUVpar(1) << std::endl;
	}

	// Store the vector normals
	for (auto& vertex : facetEntity->getNodeVector())
	{
		dataStream << "vn " << vertex.getNormal(0) << " " << vertex.getNormal(1) << " " << vertex.getNormal(2) << std::endl;
	}

	// Now store all triangles
	for (auto& triangle : facetEntity->getTriangleList())
	{
		// The vertex numbering starts at 1 in obj, but starts at 0 in the entity data structure. Furthermore, the vertex numbering is continuous for all objects in the file
		ot::UID n1 = triangle.getNode(0) + offset;
		ot::UID n2 = triangle.getNode(1) + offset;
		ot::UID n3 = triangle.getNode(2) + offset;

		dataStream << "f " << n1 << "/" << n1 << "/" << n1 << " " << n2 << "/" << n2 << "/" << n2 << " " << n3 << "/" << n3 << "/" << n3 << std::endl;
	}

	offset += facetEntity->getNodeVector().size();  // The indices are continuous.
}
