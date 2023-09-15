#pragma once

#include <string>
#include <ctime>
#include <vector>
#include <list>
#include <vtkAlgorithmOutput.h>
#include <vtkDataSetMapper.h>

#include "vtkRectilinearGrid.h"
#include "VtkDriver.h"
#include "EntityResultBase.h"
#include "PropertyBundleDataHandlePlane.h"
#include "PropertyBundleDataHandleScaling.h"
#include "PropertyBundleDataHandleVisUnstructuredVector.h"
#include "DataSourceUnstructuredMesh.h"

namespace osg {
	class Node;
}

class VtkDriverUnstructuredVectorVolume : public VtkDriver {
public:
	VtkDriverUnstructuredVectorVolume();
	virtual ~VtkDriverUnstructuredVectorVolume();

	virtual void setProperties(EntityVis2D3D *visEntity) override;
	virtual std::string buildSceneNode(DataSourceManagerItem *dataItem) override;
	
private:
	PropertyBundleDataHandlePlane * planeData = nullptr;
	PropertyBundleDataHandleScaling * scalingData = nullptr;
	PropertyBundleDataHandleVisUnstructuredVector * visData = nullptr;
	double * scalarRange = nullptr;

	vtkAlgorithmOutput* ApplyCutplane(DataSourceUnstructuredMesh *source, osg::Node *parent);
	void Assemble2DNode(vtkAlgorithmOutput * input, osg::Node *parent);
	void Assemble3DNode(DataSourceUnstructuredMesh* dataSource, osg::Node* parent);

	vtkAlgorithmOutput* AddNodeVectors2D(vtkAlgorithmOutput* input);
	vtkAlgorithmOutput* SetScalarValues(vtkAlgorithmOutput* input);
	void SetColouring(vtkPolyDataMapper* mapper);

	void CheckForModelUpdates();
	void DeletePropertyData(void);
};