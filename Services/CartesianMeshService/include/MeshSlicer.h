#pragma once

class EntityFacetData;

class MeshSlicer
{
public:
	MeshSlicer();
	virtual ~MeshSlicer();

	void doit(EntityFacetData *facets);
};

