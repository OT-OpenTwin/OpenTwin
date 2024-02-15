#include "OTCore/GenericDataStructFactory.h"
#include "OTCore/GenericDataStructMatrix.h"
#include "OTCore/GenericDataStructVector.h"
#include "OTCore/GenericDataStructSingle.h"
ot::GenericDataStruct* ot::GenericDataStructFactory::Create(const ot::ConstJsonObject& object)
{
	ot::GenericDataStruct temp;
	temp.setFromJsonObject(object);
	std::string type = temp.getTypeIdentifyer();
	if (type == ot::GenericDataStructMatrix::getClassName())
	{
		ot::GenericDataStruct* returnVal(new ot::GenericDataStructMatrix());
		returnVal->setFromJsonObject(object);
		return returnVal;
	}
	else if(type == ot::GenericDataStructVector::getClassName())
	{
		ot::GenericDataStruct* returnVal(new ot::GenericDataStructVector());
		returnVal->setFromJsonObject(object);
		return returnVal;
	}
	else if (type == ot::GenericDataStructSingle::getClassName())
	{
		ot::GenericDataStruct* returnVal(new ot::GenericDataStructSingle());
		returnVal->setFromJsonObject(object);
		return returnVal;
	}
	else
	{
		throw std::exception("Not supported GenericDataStruct type");
	}
}
