#pragma once
#include "OTCore/CoreAPIExport.h"

#include "OTCore/Tuple/TupleDescriptionComplex.h"

class TupleFactory
{
public:
	static ot::TupleDescription* create(const std::string& _tupleTypeName)
	{
		if (ot::TupleDescriptionComplex::name() == _tupleTypeName)
		{
			return new ot::TupleDescriptionComplex();
		}
		else
		{
			assert(false);
			return nullptr;
		}
	}
};

