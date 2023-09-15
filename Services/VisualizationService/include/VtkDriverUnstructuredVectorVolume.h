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

	vtkAlgorithmOutput* ApplyCutplane(osg::Node *parent);
	void Assemble2DNode(osg::Node *parent);
	void Assemble3DNode(osg::Node* parent);

	void AddNodeVectors(vtkAlgorithmOutput* input, osg::Node* parent);
	vtkAlgorithmOutput* SetScalarValues(void);
	void SetColouring(vtkPolyDataMapper* mapper);
	vtkAlgorithmOutput* GetArrowSource(void);

	void CheckForModelUpdates();
	void DeletePropertyData(void);

	DataSourceUnstructuredMesh* dataSource;
	vtkAlgorithmOutput* dataConnection;
};