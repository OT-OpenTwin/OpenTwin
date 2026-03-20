#include "ValueProcessing.h"

ValueProcessing::~ValueProcessing()
{
	for (ValueProcessor* processor : m_processors)
	{
		if (processor != nullptr)
		{
			delete processor;
			processor = nullptr;
		}
	}
}

ot::Variable ValueProcessing::executeSequence(const ot::Variable& _input)
{
	ValueProcessor* currentProcessor = *m_processors.begin();
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

void ValueProcessing::setSequence(std::list<std::unique_ptr<ValueProcessor>>&& _sequence)
{
	for (auto& processor : _sequence)
	{
		m_processors.push_back(processor.release());
	}
}

ValueProcessing ValueProcessing::createInverse()
{
	ValueProcessing inverse;
	std::list<std::unique_ptr<ValueProcessor>> processors;
	for (auto it = m_processors.rbegin(); it != m_processors.rend(); ++it)
	{
		processors.push_back((*it)->inverse());
	}
	inverse.setSequence(std::move(processors));
	return inverse;
}
