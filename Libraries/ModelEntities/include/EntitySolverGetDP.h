#pragma once
#pragma warning(disable : 4251)

#include "EntitySolver.h"
#include "OldTreeIcon.h"

#include <list>
#include <string>

class __declspec(dllexport) EntitySolverGetDP : public EntitySolver
{
public:
	EntitySolverGetDP(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, ClassFactoryHandler* factory, const std::string &owner);
	virtual ~EntitySolverGetDP();

	virtual std::string getClassName(void) override { return "EntitySolverGetDP"; } ;

	void createProperties(std::string& _meshFolderName, ot::UID& _meshFolderID, std::string& _meshName, ot::UID& _meshID);

	virtual bool updateFromProperties(void) override;

protected:
	virtual int getSchemaVersion(void) override  { return 1; } ;

private:
};

