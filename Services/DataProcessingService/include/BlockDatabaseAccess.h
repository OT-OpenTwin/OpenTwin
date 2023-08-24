#pragma once
#include "BlockDataProcessing.h"
#include "EntityBlock.h"

class BlockDatabaseAccess : public BlockDataProcessing
{
public:
	BlockDatabaseAccess();
	static std::shared_ptr<EntityBlock> CreateBlockEntity();
};
