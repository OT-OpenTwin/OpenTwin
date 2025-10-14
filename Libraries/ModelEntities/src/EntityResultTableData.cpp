#include "EntityResultTableData.h"

static EntityFactoryRegistrar<EntityResultTableData<int32_t>> registrar32("EntityResultTableData_" + ot::TypeNames::getInt32TypeName());
static EntityFactoryRegistrar<EntityResultTableData<int64_t>> registrar64("EntityResultTableData_" + ot::TypeNames::getInt64TypeName());
static EntityFactoryRegistrar<EntityResultTableData<double>> registrarDouble("EntityResultTableData_" + ot::TypeNames::getDoubleTypeName());
static EntityFactoryRegistrar<EntityResultTableData<std::string>> registrarString("EntityResultTableData_" + ot::TypeNames::getStringTypeName());