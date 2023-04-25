#pragma once
#pragma warning(disable : 4251)

#include "EntityBase.h"
#include "Types.h"

#include <list>


class __declspec(dllexport) EntityMeshTetItemTet
{
public:
	EntityMeshTetItemTet() {};
	virtual ~EntityMeshTetItemTet() {};

	void setNumberOfNodes(int n) { assert(nodes.empty()); nodes.resize(n); }
	int getNumberOfNodes(void) { return (int)nodes.size(); }

	void   setNode(int i, size_t n) { assert(i < (int)nodes.size()); nodes[i] = n; };
	size_t getNode(int i) { assert(i < (int)nodes.size()); return nodes[i]; };

private:
	std::vector<size_t> nodes;
};

class __declspec(dllexport) EntityMeshTetItemDataTets : public EntityBase
{
public:
	EntityMeshTetItemDataTets(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactory *factory, const std::string &owner);
	virtual ~EntityMeshTetItemDataTets();

	virtual bool getEntityBox(double &xmin, double &xmax, double &ymin, double &ymax, double &zmin, double &zmax) override;

	void setNumberTets(size_t nT) { tets.resize(nT); setModified(); };
	size_t getNumberTets(void) { return tets.size(); };

	void setNumberOfTetNodes(size_t nT, int nNodes) { tets[nT].setNumberOfNodes(nNodes); };
	void setTetNode(size_t nT, int nodeIndex, size_t node) { assert(nT < tets.size()); tets[nT].setNode(nodeIndex, node); setModified(); };
	size_t getTetNode(size_t nT, int nodeIndex) { assert(nT < tets.size()); return tets[nT].getNode(nodeIndex); };

	virtual std::string getClassName(void) { return "EntityMeshTetItemDataTets"; };

	virtual entityType getEntityType(void) override { return DATA; };

private:
	virtual int getSchemaVersion(void) { return 1; };
	virtual void AddStorageData(bsoncxx::builder::basic::document &storage);
	virtual void readSpecificDataFromDataBase(bsoncxx::document::view &doc_view, std::map<ot::UID, EntityBase *> &entityMap) override;

	std::vector<EntityMeshTetItemTet> tets;
};



