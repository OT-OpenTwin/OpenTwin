#pragma
#include "OpenTwinFoundation/BusinessLogicHandler.h"

class ValidityHandler : public BusinessLogicHandler
{
public:
	bool blockDiagramIsValid();

private:
	bool hasCycle();

};
