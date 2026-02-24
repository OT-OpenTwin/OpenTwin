#pragma once
#include "OTCore/CoreAPIExport.h"

#include "OTCore/Tuple/TupleDescriptionComplex.h"

class  TupleFactory
{
public:
	static TupleDescription* create(const std::string& _tupleTypeName)
	{
		if (TupleDescriptionComplex::m_name == _tupleTypeName)
		{
			return new TupleDescriptionComplex();
		}
		else
		{
			assert(false);
			return nullptr;
		}
	}
};

