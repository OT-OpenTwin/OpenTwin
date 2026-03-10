#include "ValueProcessing.h"

ot::Variable ValueProcessing::executeSequence(const ot::Variable& _input)
{
	ValueProcessor* currentProcessor = &m_startProcessor;
	ot::Variable currentValue = _input;
	while (currentProcessor != nullptr)
	{
		currentValue = currentProcessor->execute(currentValue);
		currentProcessor = currentProcessor->getNext();

	}

	return currentValue;
}

void ValueProcessing::setSequence(const std::string& _jsonSerialisedSequence)
{

}
