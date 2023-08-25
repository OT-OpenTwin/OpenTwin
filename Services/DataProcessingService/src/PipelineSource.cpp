#include "PipelineSource.h"
#include "EntityBlockDatabaseAccess.h"
#include "BlockHandlerDatabaseAccess.h"

PipelineSource::PipelineSource(std::shared_ptr<EntityBlock> blockEntity)
    : PipelineItem(blockEntity), _handler(nullptr)
{
    EntityBlockDatabaseAccess* dbA = dynamic_cast<EntityBlockDatabaseAccess*>(_blockEntity.get());
    if (dbA != nullptr)
    {
        _handler = new BlockHandlerDatabaseAccess(dbA);
    }
}

PipelineItem::genericDataBlock PipelineSource::CreateData(genericDataBlock& parameter) const
{
    return _handler->Execute(parameter);
}
