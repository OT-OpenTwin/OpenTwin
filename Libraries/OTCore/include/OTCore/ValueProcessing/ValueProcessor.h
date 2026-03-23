// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/InClassFactory.h"
#include "OTCore/Serializable.h"
#include "OTCore/Math.h"
#include "OTCore/Variable/Variable.h"

// std header
#include <memory>

namespace ot {

	class OT_CORE_API_EXPORT ValueProcessor : public Serializable
	{
		OT_DECL_DEFCOPY(ValueProcessor)
		OT_DECL_DEFMOVE(ValueProcessor)
		OT_DECL_INCLASS_FACTORY(ValueProcessor, ValueProcessor, Registrar)
	public:

		//! @brief Create a ValueProcessor object from the provided JSON string.
		//! The caller takes ownership of the created ValueProcessor object.
		//! @param _jsonString JSON string containing the information to create the ValueProcessor object.
		//! The JSON string has to contain a member "ClassName" with the name of the ValueProcessor subclass to create.
		//! @throws DeserializationException If the JSON string cannot be parsed.
		//! @throws InvalidArgumentException If the JSON string does not contain a member "ClassName" or if the class name is unknown.
		//! @ref ValueProcessor::fromJson(const ConstJsonObject& _jsonObject)
		OT_DECL_NODISCARD static ValueProcessor* fromJson(const std::string& _jsonString);

		//! @brief Create a ValueProcessor object from the provided JSON object.
		//! The caller takes ownership of the created ValueProcessor object.
		//! @param _jsonObject JSON object containing the information to create the ValueProcessor object.
		//! The JSON object has to contain a member "ClassName" with the name of the ValueProcessor subclass to create.
		//! @throws InvalidArgumentException If the JSON object does not contain a member "ClassName" or if the class name is unknown.
		OT_DECL_NODISCARD static ValueProcessor* fromJson(const ConstJsonObject& _jsonObject);

		ValueProcessor() = default;
		virtual ~ValueProcessor() = default;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Processing

		virtual ot::Variable execute(const ot::Variable& _input) = 0;
		OT_DECL_NODISCARD virtual ValueProcessor* inverse() const = 0;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Serialization

		OT_DECL_NODISCARD virtual ValueProcessor* createCopy() const = 0;
		virtual std::string getClassName() const = 0;

		virtual void addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const override;
		virtual void setFromJsonObject(const ConstJsonObject& _jsonObject) override;
	};

}

