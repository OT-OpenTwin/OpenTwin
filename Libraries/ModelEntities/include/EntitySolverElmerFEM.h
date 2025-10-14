#pragma once
#pragma warning(disable : 4251)

#include "EntitySolver.h"
#include "OldTreeIcon.h"

#include <list>
#include <string>

class __declspec(dllexport) EntitySolverElmerFEM : public EntitySolver
{
public:
	EntitySolverElmerFEM() : EntitySolverElmerFEM(0, nullptr, nullptr, nullptr, "") {};
	EntitySolverElmerFEM(ot::UID ID, EntityBase *parent, EntityObserver *obs, ModelState *ms, const std::string &owner);
	virtual ~EntitySolverElmerFEM();

	virtual std::string getClassName(void) override { return "EntitySolverElmerFEM"; } ;

	void createProperties(std::string& _meshFolderName, ot::UID& _meshFolderID, std::string& _meshName, ot::UID& _meshID);

	virtual bool updateFromProperties(void) override;

protected:
	virtual int getSchemaVersion(void) override  { return 1; } ;

private:
};

