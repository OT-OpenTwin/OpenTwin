// @otlicense

#include "FixtureNumpyAPI.h"

#define PY_ARRAY_UNIQUE_SYMBOL FixtureNumpyAPI_ARRAY_API
#define NPY_NO_DEPRECATED_API NPY_1_7_API_VERSION
#include "numpy/ndarrayobject.h"

FixtureNumpyAPI::FixtureNumpyAPI()
{
	Py_Initialize();
	int errorCode =	initiateNumpy();
}

int FixtureNumpyAPI::initiateNumpy()
{
	import_array1(0);
	return 1;
}

FixtureNumpyAPI::~FixtureNumpyAPI()
{
	Py_Finalize();
}
