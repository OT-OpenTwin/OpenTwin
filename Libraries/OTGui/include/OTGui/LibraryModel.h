//! @file LibraryModel.h
//! @author Sebastian Urmann	
//! @date June 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/LogDispatcher.h"
#include "OTGui/GuiTypes.h"

// std header
#include <string>
#include <unordered_map>

namespace ot {

	class OT_GUI_API_EXPORT LibraryModel : public Serializable {
		OT_DECL_DEFCOPY(LibraryModel)
		OT_DECL_DEFMOVE(LibraryModel)
	public:
		LibraryModel() = default;
		LibraryModel(const std::string& _name, const std::string& _modelType, const std::string& _elementType);
		virtual ~LibraryModel() = default;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void setName(const std::string& _name) { m_name = _name; };
		const std::string& getName() const { return m_name; }

		void setModelType(const std::string& _modelType) { m_modelType = _modelType; };
		const std::string& getModeltype() const { return m_modelType; }

		void setElementType(const std::string& _modelType) { m_elementType = m_elementType; };
		const std::string& getElementType() const { return m_elementType; }

		void addMetaData(const std::string& _key, const std::string& _value);
		bool hasMetaDataValue(const std::string& _key) const { return m_metaData.find(_key) != m_metaData.end(); };
		std::string getMetaDataValue(const std::string& _key) const;
		const std::unordered_map<std::string, std::string>& getMetaData() const { return m_metaData; };

		// ###########################################################################################################################################################################################################################################################################################################################

		// Serialization

		//! @brief Add the object contents to the provided JSON object
		//! @param _object Json object reference
		//! @param _allocator Allocator
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

	private:
		std::string m_name;
		std::string m_modelType;
		std::string m_elementType;
		std::unordered_map<std::string, std::string> m_metaData;
	};
}