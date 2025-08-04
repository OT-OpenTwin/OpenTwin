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

