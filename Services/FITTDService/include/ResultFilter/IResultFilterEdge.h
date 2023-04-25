#pragma once
#include "Ports/SourceTarget.h"

class IResultFilterEdgeSpecific
{
public:
	virtual void SetEdgeSepcialization(axis targetComponent) =0;
};