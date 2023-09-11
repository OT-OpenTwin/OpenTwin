#pragma once
#include <string>
#include <functional>
#include "EntityBlock.h"
#include "BlockEntityHandler.h"

struct BlockEntityFactoryRegistrar
{
	BlockEntityFactoryRegistrar(const std::string& name, std::function<std::shared_ptr<EntityBlock>()> factoryMethod)
	{
		BlockEntityHandler::GetInstance().RegisterBlockEntity(name, factoryMethod);
	};
};
