// @otlicense

#pragma once
#pragma warning(disable : 4251)

#include "EntityBase.h"
#include "OldTreeIcon.h"

#include <list>
#include <vector>

class __declspec(dllexport) EntityMeshTetItemTetEdge
{
public:
	EntityMeshTetItemTetEdge() : nodes{ 0, 0 } {};
	virtual ~EntityMeshTetItemTetEdge() {};

	void setNodes(size_t n[2]) { nodes[0] = n[0]; nodes[1] = n[1]; };
	void getNodes(size_t n[2]) { n[0] = nodes[0]; n[1] = nodes[1]; };

	size_t getNode(int index) { return nodes[index]; }
	void setNode(int index, size_t node) { nodes[index] = node; }

private:
	size_t nodes[2];
};

class __declspec(dllexport) EntityMeshTetItemDataTetedges : public EntityBase
{
public:
	EntityMeshTetItemDataTetedges() : EntityMeshTetItemDataTetedges(0, nullptr, nullptr, nullptr, "") {};
	EntityMeshTetItemDataTetedges(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, const std::string &owner);
	virtual ~EntityMeshTetItemDataTetedges();

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	void setNumberEdges(size_t nT) { edges.resize(nT); setModified(); };
	size_t getNumberEdges(void) { return edges.size(); };

	void setEdgeNodes(size_t nE, size_t n[2]) { assert(nE < edges.size()); edges[nE].setNodes(n); setModified(); };
	void getEdgeNodes(size_t nE, size_t n[2]) { assert(nE < edges.size()); edges[nE].getNodes(n); };

	virtual std::string getClassName(void) const override { return "EntityMeshTetItemDataTetedges"; };

	virtual entityType getEntityType(void) const override { return DATA; };

private:
	virtual int getSchemaVersion(void) override { return 1; };
	virtual void addStorageData(bsoncxx::builder::basic::document &storage) override;
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;

	std::vector<EntityMeshTetItemTetEdge> edges;
};



