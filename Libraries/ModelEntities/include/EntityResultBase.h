#pragma once
#pragma warning(disable : 4251)

#include "Types.h"

#include <list>
#include <vector>

class __declspec(dllexport) EntityResultBase
{
public:
	enum tResultType
	{
		UNKNOWN = 0,
		CARTESIAN_EDGES_3D = 1,
		CARTESIAN_FACES_3D = 2,
		CARTESIAN_DUAL_EDGES_3D = 3,
		CARTESIAN_DUAL_FACES_3D = 4,
		CARTESIAN_NODE_VECTORS = 5,
		CARTESIAN_DUAL_NODE_VECTORS = 6,
		UNSTRUCTURED_SCALAR = 7,
		UNSTRUCTURED_VECTOR = 8
	};

	EntityResultBase() : resultType(UNKNOWN) {};
	virtual ~EntityResultBase() {};

	void setResultType(tResultType type) { resultType = type; }
	tResultType getResultType(void) { return resultType; }

private:
	tResultType resultType;
};


