// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/Serializable.h"

namespace ot
{

	class OT_CORE_API_EXPORT ProjectCompareConfig : public Serializable
	{
		OT_DECL_DEFCOPY(ProjectCompareConfig)
		OT_DECL_DEFMOVE(ProjectCompareConfig)
	public:

		//! @brief Flags for project comparison.
		//! 
		//! @note Do not forget to update the toString and stringToProjectCompareFlag methods when adding new flags.
		//! 
		enum class ProjectCompareFlag
		{
			NoFlags           = 0 << 0,
			CompareProperties = 1 << 0,
			CompareData       = 1 << 1,
			CompareResults    = 1 << 2,
			
			Iterator_First    = CompareProperties, //! @brief First valid non null value.
			Iterator_Last     = CompareResults     //! @brief Last valid non null value.
		};
		typedef Flags<ProjectCompareFlag> ProjectCompareFlags;

		static std::string toString(ProjectCompareFlag _flag);
		static ProjectCompareFlag stringToProjectCompareFlag(const std::string& _flagString);
		static std::list<std::string> toStringList(const ProjectCompareFlags& _flags);
		static ProjectCompareFlags stringListToProjectCompareFlags(const std::list<std::string>& _flagStrings);

		ProjectCompareConfig() = default;
		ProjectCompareConfig(const ConstJsonObject& _jsonObject);
		virtual ~ProjectCompareConfig() = default;
		
		void addToJsonObject(JsonValue& _jsonObject, JsonAllocator& _allocator) const override;
		void setFromJsonObject(const ConstJsonObject& _jsonObject) override;
		
		void setTargetProjectName(const std::string& _name) { m_targetProjectName = _name; };
		const std::string& getTargetProjectName() const { return m_targetProjectName; };

		void setTargetProjectVersion(const std::string& _version) { m_targetProjectVersion = _version; };
		const std::string& getTargetProjectVersion() const { return m_targetProjectVersion; };

		void setFlag(ProjectCompareFlag _flag, bool _enabled = true) { m_flags.set(_flag, _enabled); };
		void setFlags(const ProjectCompareFlags& _flags) { m_flags = _flags; };
		const ProjectCompareFlags& getFlags() const { return m_flags; };

	private:
		std::string m_targetProjectName;
		std::string m_targetProjectVersion;
		ProjectCompareFlags m_flags = ProjectCompareFlag::NoFlags;

	};

}