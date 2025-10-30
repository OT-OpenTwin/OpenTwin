// @otlicense
// File: DataSourceManagerItem.cpp
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


#include "stdafx.h"

#include "DataSourceManagerItem.h"
#include "EntityBase.h"

#include <vtkNew.h>
#include <vtkStructuredGrid.h>
#include <vtkRectilinearGrid.h>
#include <vtkStructuredGridReader.h>
#include <vtkDataArray.h>
#include <vtkDoubleArray.h>
#include <vtkPointData.h>
#include <vtkFloatArray.h>
#include <vtkCell.h>


DataSourceManagerItem::DataSourceManagerItem() 
{
}

DataSourceManagerItem::~DataSourceManagerItem()
{
}



/*
vtkDataObject *DataSourceManagerItem::getDataSource(void)
{
	if (dataSource == nullptr) return nullptr;

	return dataSource;
}

double *DataSourceManagerItem::GetScalarRange(void)
{
	if (dataSource == nullptr) return nullptr;

	return dataSource->GetScalarRange();
}
*/

//def writeVTR(vtr_name, scalar_fields, vector_fields, Nxc, Nyc, Nzlocal, 
//	vtkXcoordinates, vtkYcoordinates, vtkZcoordinates, rankproc, numproc):
//	"""Writes a single VTR file per Python processor/variable
//
//	"""
//	rtg = vtk.vtkRectilinearGrid()
//	rtg.SetDimensions(Nxc+1, Nyc+1, Nzlocal)
//	rtg.SetExtent(0, Nxc, 0, Nyc, rankproc*Nzc/numproc, (rankproc+1)*Nzc/numproc)
//	rtg.SetXCoordinates(vtkXcoordinates)
//	rtg.SetYCoordinates(vtkYcoordinates)
//	rtg.SetZCoordinates(vtkZcoordinates)

//	vtk_data = []
//	array_list = []

//	for f in scalar_fields:
//		vtk_data.append(vtk.vtkFloatArray())
//		vtk_data[-1].SetNumberOfTuples(numpoints)
//		vtk_data[-1].SetNumberOfComponents(1)
//		array_list.append(scalar_fields[f].swapaxes(0,2).flatten().astype("float32"))
//		vtk_data[-1].SetVoidArray(array_list[-1], numpoints, 1)
//		vtk_data[-1].SetName(f)
//		if f == scalar_fields.keys()[0]:
//			rtg.GetPointData().SetScalars(vtk_data[-1])
//		else:
//			rtg.GetPointData().AddArray(vtk_data[-1])
//	#end for

//	for f in vector_fields:
//		vtk_data.append(vtk.vtkFloatArray())
//		vtk_data[-1].SetNumberOfTuples(numpoints*3)
//		vtk_data[-1].SetNumberOfComponents(3)
//		array_list.append(vector_fields[f].swapaxes(0,3).swapaxes(1,2).flatten().astype("float32"))
//		vtk_data[-1].SetVoidArray(array_list[-1], numpoints*3, 1)
//		vtk_data[-1].SetName(f)
//		if f == vector_fields.keys()[0]:
//			rtg.GetPointData().SetVectors(vtk_data[-1])
//		else:
//			rtg.GetPointData().AddArray(vtk_data[-1])
//	#end for

//try:
//	writer = vtk.vtkXMLRectilinearGridWriter()                                                                                              
//	writer.SetFileName(vtr_name)
//	writer.SetInput(rtg)
//	writer.Write()
//	except:
//	print 'Error writing VTR file: ', vtr_name