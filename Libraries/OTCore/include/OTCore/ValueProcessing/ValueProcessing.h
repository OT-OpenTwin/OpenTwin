// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/Variable/Variable.h"
#include "OTCore/ValueProcessing/ValueProcessor.h"

// std header
#include <list>

// Conversion types
//| Type		| Formula			| Example		| Special Handling   |
//| ----------	| --------------	| --------		| ------------------ |
//| Linear		| ax				| m <-> km		| standard           |
//| Affine		| ax + b			| degree C <-> K| needs offset       |
//| Nonlinear	| f(x)				| dB			| special math       |
//| Power		| pow(a,n)			| cm2 <-> m2	| exponentiate       |
//| Composite	| product			| km / h		| base reduction     |
// function has to be strictly monotonically increasing or decreasing. 

namespace ot {

	class OT_CORE_API_EXPORT ValueProcessing : public Serializable
	{
	public:
		static ValueProcessing fromJson(const std::string& _jsonString);

		explicit ValueProcessing() = default;
		explicit ValueProcessing(const ConstJsonObject& _jsonObject);
		ValueProcessing(const ValueProcessing& _other);
		ValueProcessing(ValueProcessing&& _other) noexcept;
		virtual ~ValueProcessing();

		ValueProcessing& operator=(const ValueProcessing& _other);
		ValueProcessing& operator=(ValueProcessing&& _other) noexcept;

		virtual void addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _jsonObject) override;

		//! @brief Execute the sequence of value processors on the input variable.
		//! @param _input Input variable to process through the sequence of value processors.
		//! @return The result of processing the input variable through the sequence of value processors.
		OT_DECL_NODISCARD ot::Variable execute(const ot::Variable& _input) const;

		bool isEmpty() const { return m_processors.empty(); };

		OT_DECL_NODISCARD ValueProcessing inverse() const;

		void clear();

		//! @brief Add a processor to the end of the sequence.
		//! The object takes ownership of the provided processor.
		//! @param _processor Processor to add to the end of the sequence. The object takes ownership of the provided processor.
		void addBack(ValueProcessor* _processor) { m_processors.push_back(_processor); };

		//! @brief Add a processor to the front of the sequence.
		//! The object takes ownership of the provided processor.
		//! @param _processor Processor to add to the front of the sequence. The object takes ownership of the provided processor.
		void addFront(ValueProcessor* _processor) { m_processors.push_front(_processor); };

	private:
		std::list<ValueProcessor*> m_processors;
	};

}

// target = "m/s", unit = "km/h" -> bool processible = parse(unit, target) -> ValueProcessor::Linear(1000), ValueProcessor::Linear(1/3600)
// if not processible & target unit != query unit -> User feedback: No standart unit, no transfer possible
// ToDo: prevent Olf at the data ingestion.
