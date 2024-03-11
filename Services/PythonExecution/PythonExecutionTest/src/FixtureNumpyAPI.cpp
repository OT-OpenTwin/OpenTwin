#include "FixtureNumpyAPI.h"

FixtureNumpyAPI::FixtureNumpyAPI()
{
	Py_Initialize();
}

FixtureNumpyAPI::~FixtureNumpyAPI()
{
	Py_Finalize();
}
