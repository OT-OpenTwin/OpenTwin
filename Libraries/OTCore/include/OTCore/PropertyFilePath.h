//! @file PropertyFilePath.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Property.h"

// std header
#include <string>

#define OT_PROPERTY_TYPE_FilePath "FilePath"

namespace ot {

	class OT_CORE_API_EXPORT PropertyFilePath : public Property {
		OT_DECL_NOCOPY(PropertyFilePath)
	public:
		enum BrowseMode {
			ReadFile = 0,
			WriteFile = 1
		};

		struct FilterInfo {
			std::string extension;
			std::string text;
		};

		PropertyFilePath(const PropertyFilePath* _other);
		PropertyFilePath(const PropertyBase& _base);
		PropertyFilePath(BrowseMode _mode = ReadFile, PropertyFlags _flags = PropertyFlags(NoFlags));
		PropertyFilePath(const std::string& _path, BrowseMode _mode = ReadFile, PropertyFlags _flags = PropertyFlags(NoFlags));
		PropertyFilePath(const std::string& _name, const std::string& _path, BrowseMode _mode = ReadFile, PropertyFlags _flags = PropertyFlags(NoFlags));
		virtual ~PropertyFilePath() {};

		virtual std::string getPropertyType(void) const override { return OT_PROPERTY_TYPE_FilePath; };

		virtual Property* createCopy(void) const override;

		void setPath(const std::string& _path) { m_path = _path; };
		const std::string& path(void) const { return m_path; };

		void setBrowseMode(BrowseMode _mode) { m_browseMode = _mode; };
		BrowseMode browseMode(void) const { return m_browseMode; };

		void setFilters(const std::list<FilterInfo>& _filters) { m_filters = _filters; };
		void addFilter(const std::string& _extension, const std::string& _text) { this->addFilter(FilterInfo{ _extension, _text }); };
		void addFilter(const FilterInfo& _info);
		const std::list<FilterInfo>& filters(void) const { return m_filters; }

	protected:
		//! @brief Add the property data to the provided JSON object
		//! The property type is already added
		//! @param _object Json object reference
		//! @param _allocator Allocator
		virtual void getPropertyData(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Set the property data from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setPropertyData(const ot::ConstJsonObject& _object) override;

	private:
		std::string m_path;
		BrowseMode m_browseMode;
		std::list<FilterInfo> m_filters;
	};

}