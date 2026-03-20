#pragma once
#include "OTCore/Variable/Variable.h"
#include "ValueProcessor.h"


// Conversion types
//| Type		| Formula			| Example		| Special Handling   |
//| ----------	| --------------	| --------		| ------------------ |
//| Linear		| ax				| m <-> km		| standard           |
//| Affine		| ax + b			| degree C <-> K| needs offset       |
//| Nonlinear	| f(x)				| dB			| special math       |
//| Power		| pow(a,n)			| cm2 <-> m2	| exponentiate       |
//| Composite	| product			| km / h		| base reduction     |
// function has to be strictly monotonically increasing or decreasing. 
class ValueProcessing
{
public:
	~ValueProcessing();
	ValueProcessing() = default;
	ValueProcessing(const ValueProcessing& _other);
	ValueProcessing(ValueProcessing&& _other) noexcept;
	ValueProcessing& operator=(const ValueProcessing& _other);
	ValueProcessing& operator=(ValueProcessing&& _other) noexcept;

	ot::Variable executeSequence(const ot::Variable& _input) const;
	void setSequence(const std::string& _jsonSerialisedSequence);
	void setSequence(std::list<std::unique_ptr<ValueProcessor>>&& _sequence);
	bool executionNecessary() const { return m_processors.size() > 0; }
	ValueProcessing createInverse() const;

private:
	std::list<ValueProcessor*> m_processors;
};





//target = "m/s", unit = "km/h" -> bool processible = parse(unit, target) -> ValueProcessor::Linear(1000), ValueProcessor::Linear(1/3600)
// if not processible & target unit != query unit -> User feedback: No standart unit, no transfer possible
// ToDo: prevent Olf at the data ingestion.
