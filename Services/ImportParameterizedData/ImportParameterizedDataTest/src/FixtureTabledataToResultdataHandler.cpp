// @otlicense

#include "FixtureTabledataToResultdataHandler.h"
#include "TabledataToResultdataHandler.h"

std::string FixtureTabledataToResultdataHandler::extractUnitFromName(std::string& name)
{
	
	TabledataToResultdataHandler handler("", "", "", "", "");
	return handler.extractUnitFromName(name);
}
