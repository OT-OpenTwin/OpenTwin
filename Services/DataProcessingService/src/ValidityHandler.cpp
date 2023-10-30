#include "ValidityHandler.h"

bool ValidityHandler::blockDiagramIsValid()
{
	

	if (hasCycle())
	{
		return false;
	}
	return true;
}

bool ValidityHandler::hasCycle()
{

	return false;
}
