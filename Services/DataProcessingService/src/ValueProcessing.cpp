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

ValueProcessing::ValueProcessing(const ValueProcessing& _other)
{
	for (auto& processor : _other.m_processors)
	{
		m_processors.push_back(processor->createCopy());
	}
}

ValueProcessing::ValueProcessing(ValueProcessing&& _other) noexcept
{
	for (auto& processor : _other.m_processors)
	{
		m_processors.push_back(std::move(processor));
	}
	_other.m_processors.clear();
}

ValueProcessing& ValueProcessing::operator=(const ValueProcessing& _other)
{
	for (auto& processor : _other.m_processors)
	{
		m_processors.push_back(processor->createCopy());
	}
	return *this;
}

ValueProcessing& ValueProcessing::operator=(ValueProcessing&& _other) noexcept 
{
	for (auto& processor : _other.m_processors)
	{
		m_processors.push_back(std::move(processor));
	}
	_other.m_processors.clear();
	return *this;
}

ot::Variable ValueProcessing::executeSequence(const ot::Variable& _input) const
{
	ot::Variable currentValue = _input;
	for (ValueProcessor* currentProcessor : m_processors)
	{
		currentValue = currentProcessor->execute(currentValue);
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

ValueProcessing ValueProcessing::createInverse() const 
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
