#pragma once
#include "OTResultDataAccess/ResultDataAccessAPIExport.h"

#include "OTResultDataAccess/SerialisationInterfaces/TupleDescriptionComplex.h"

class OT_RESULTDATAACCESS_API_EXPORT TupleFactory
{
public:
	static TupleDescription* create(const std::string& _formatName)
	{
		if (TupleDescriptionComplex::m_name == _formatName)
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

