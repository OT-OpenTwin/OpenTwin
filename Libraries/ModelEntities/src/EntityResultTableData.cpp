#include "EntityResultTableData.h"

static EntityFactoryRegistrar<EntityResultTableData<int32_t>> registrar32(EntityResultTableData<int32_t>::classNameBase() + "_" + ot::TypeNames::getInt32TypeName());
static EntityFactoryRegistrar<EntityResultTableData<int64_t>> registrar64(EntityResultTableData<int64_t>::classNameBase() + "_" + ot::TypeNames::getInt64TypeName());
static EntityFactoryRegistrar<EntityResultTableData<double>> registrarDouble(EntityResultTableData<double>::classNameBase() + "_" + ot::TypeNames::getDoubleTypeName());
static EntityFactoryRegistrar<EntityResultTableData<std::string>> registrarString(EntityResultTableData<std::string>::classNameBase() + "_" + ot::TypeNames::getStringTypeName());