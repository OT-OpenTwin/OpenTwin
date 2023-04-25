
#include "stdafx.h"

#include "VtkDriver.h"
#include "DataSourceManager.h"
#include "DataSourceManagerItem.h"
#include "EntityVis2D3D.h"

#include <osg/StateSet>
#include <osg/Node>
#include <osg/BlendFunc>
#include <osg/CullFace>
#include <osg/Switch>
#include <osg/Geometry>
#include <osg/Geode>
#include <osg/PolygonOffset>
#include <osg/PolygonMode>
#include <osg/LineWidth>
#include <osg/LightModel>
#include <osg/LightSource>

#include <osg/Image>
#include <osg/Texture2D>
#include <osgDB/ReadFile>

#include "vtk2osg.h"

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCylinderSource.h>
#include <vtkConeSource.h>
#include <vtkCubeSource.h>
#include <vtkNamedColors.h>
#include <vtkNew.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkCallbackCommand.h>
#include <vtkPolyDataNormals.h>
#include <vtkPolyData.h>
#include <vtkBandedPolyDataContourFilter.h>
#include <vtkFloatArray.h>
#include <vtkPointData.h>
#include <vtkStructuredGridReader.h>
#include <vtkPlane.h>
#include <vtkStructuredGrid.h>
#include <vtkCutter.h>
#include <vtkArrowSource.h>
#include <vtkGlyph3D.h>
#include <vtkLookupTable.h>
#include <vtkExtractRectilinearGrid.h>

VtkDriver::VtkDriver() 
{

}

VtkDriver::~VtkDriver()
{

}

void VtkDriver::reportTime(const std::string &message, std::time_t &timer)
{
	double seconds = difftime(time(nullptr), timer);

	std::cout << message << ": " << std::setprecision(6) << seconds << " sec." << std::endl;

	timer = time(nullptr);
}

