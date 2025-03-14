//! @file PropertyPath.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/Property.h"

// std header
#include <string>

namespace ot {

	class OT_GUI_API_EXPORT PropertyPath : public Property {
		OT_DECL_NOCOPY(PropertyPath)
	public:
		typedef std::string FilePathValueType;
		using PropertyValueType = FilePathValueType;

		struct FilterInfo {
			std::string extension;
			std::string text;
		};

		PropertyPath(const PropertyPath* _other);
		PropertyPath(const PropertyBase& _base);
		PropertyPath(PathBrowseMode _mode = PathBrowseMode::ReadFile, PropertyFlags _flags = PropertyFlags(NoFlags));
		PropertyPath(const std::string& _path, PathBrowseMode _mode = PathBrowseMode::ReadFile, PropertyFlags _flags = PropertyFlags(NoFlags));
		PropertyPath(const std::string& _name, const std::string& _path, PathBrowseMode _mode = PathBrowseMode::ReadFile, PropertyFlags _flags = PropertyFlags(NoFlags));
		virtual ~PropertyPath() {};

		static std::string propertyTypeString(void) { return "FilePath"; };
		virtual std::string getPropertyType(void) const override { return PropertyPath::propertyTypeString(); };

		virtual void mergeWith(const Property* _other, const MergeMode& _mergeMode) override;

		virtual Property* createCopy(void) const override;

		void setPath(const std::string& _path) { m_path = _path; };
		const std::string& getPath(void) const { return m_path; };

		void setBrowseMode(PathBrowseMode _mode) { m_browseMode = _mode; };
		PathBrowseMode getBrowseMode(void) const { return m_browseMode; };

		void setFilters(const std::list<FilterInfo>& _filters) { m_filters = _filters; };
		void addFilter(const std::string& _extension, const std::string& _text) { this->addFilter(FilterInfo{ _extension, _text }); };
		void addFilter(const FilterInfo& _info);
		const std::list<FilterInfo>& getFilters(void) const { return m_filters; }

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
		PathBrowseMode m_browseMode;
		std::list<FilterInfo> m_filters;
	};

}