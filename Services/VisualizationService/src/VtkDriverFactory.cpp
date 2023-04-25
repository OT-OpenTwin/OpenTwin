#include "VtkDriverFactory.h"

#include "VtkDriver.h"
#include "EntityVis2D3D.h"

#include "VtkDriverGeneric.h"
#include "VtkDriverCartesianFaceScalar.h"
#include "VtkDriverCartesianVectorfield.h"

VtkDriver *VtkDriverFactory::createDriver(EntityVis2D3D *visEntity)
{
	if (visEntity->getClassName() == "EntityVisCartesianFaceScalar")
	{
		return new VtkDriverCartesianFaceScalar;
	}
	else if (visEntity->getClassName() == "EntityResult3D") {
		return new VtkDriverCartesianVectorfield;
	}
	else
	{
		assert(0); // Unknown data type
	}

	return new VtkDriverGeneric;
}

