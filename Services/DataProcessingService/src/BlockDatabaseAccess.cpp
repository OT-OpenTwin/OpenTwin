#include "BlockDatabaseAccess.h"
#include "EntityBlockDatabaseAccess.h"

BlockDatabaseAccess::BlockDatabaseAccess()
	:BlockDataProcessing("Database access", "Database Access", "C", "BlockDataBaseAccess")
{}

std::shared_ptr<EntityBlock> BlockDatabaseAccess::CreateBlockEntity()
{
	std::shared_ptr<EntityBlockDatabaseAccess> dbAccessBlockEntity(new EntityBlockDatabaseAccess(0, nullptr, nullptr, nullptr, nullptr, ""));

	std::list<std::string> listOfProjects{ "Test" };
	dbAccessBlockEntity->createProperties(listOfProjects, *listOfProjects.begin());
	return dbAccessBlockEntity;
}
