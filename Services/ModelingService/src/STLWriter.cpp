// @otlicense
// File: STLWriter.cpp
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

#include "STLWriter.h"
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

void STLWriter::initializeWriter()
{
}

void STLWriter::appendData(const std::string &objectName, EntityFacetData* facetEntity, std::stringstream &dataStream)
{
	dataStream << "solid " << objectName << "\n";

	for (auto& triangle : facetEntity->getTriangleList())
	{
		ot::UID n1 = triangle.getNode(0);
		ot::UID n2 = triangle.getNode(1);
		ot::UID n3 = triangle.getNode(2);

		gp_Pnt p1(facetEntity->getNodeVector()[n1].getCoord(0), facetEntity->getNodeVector()[n1].getCoord(1), facetEntity->getNodeVector()[n1].getCoord(2));
		gp_Pnt p2(facetEntity->getNodeVector()[n2].getCoord(0), facetEntity->getNodeVector()[n2].getCoord(1), facetEntity->getNodeVector()[n2].getCoord(2));
		gp_Pnt p3(facetEntity->getNodeVector()[n3].getCoord(0), facetEntity->getNodeVector()[n3].getCoord(1), facetEntity->getNodeVector()[n3].getCoord(2));

		gp_Vec v1(p1, p2); // p2 - p1
		gp_Vec v2(p1, p3); // p3 - p1

		gp_Vec n = v1.Crossed(v2); // (p2-p1) x (p3-p1)

		gp_Dir normal;

		if (n.Magnitude() > Precision::Confusion())
		{
			normal = gp_Dir(n);
		}

		writeFacet(dataStream, p1.X(), p1.Y(), p1.Z(), p2.X(), p2.Y(), p2.Z(), p3.X(), p3.Y(), p3.Z(), n.X(), n.Y(), n.Z());
	}

	dataStream << "endsolid " << objectName << "\n";
}

void STLWriter::writeFacet(std::stringstream& output, double v1x, double v1y, double v1z, double v2x, double v2y, double v2z, double v3x, double v3y, double v3z, double nx, double ny, double nz)
{
	// Write a single triangle to the output stream in STL format

	output << "  facet normal " << nx << " " << ny << " " << nz << "\n";
	output << "    outer loop\n";
	output << "      vertex " << v1x << " " << v1y << " " << v1z << "\n";
	output << "      vertex " << v2x << " " << v2y << " " << v2z << "\n";
	output << "      vertex " << v3x << " " << v3y << " " << v3z << "\n";
	output << "    endloop\n";
	output << "  endfacet\n";
}
