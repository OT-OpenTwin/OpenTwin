#include "EntityResultTable.h"

static EntityFactoryRegistrar<EntityResultTable<int32_t>> registrar32("EntityResultTable_" + ot::TypeNames::getInt32TypeName());
static EntityFactoryRegistrar<EntityResultTable<int64_t>> registrar64("EntityResultTable_" + ot::TypeNames::getInt64TypeName());
static EntityFactoryRegistrar<EntityResultTable<double>> registrarDouble("EntityResultTable_" + ot::TypeNames::getDoubleTypeName());
static EntityFactoryRegistrar<EntityResultTable<std::string>> registrarString("EntityResultTable_" + ot::TypeNames::getStringTypeName());