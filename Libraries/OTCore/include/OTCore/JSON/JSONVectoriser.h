//@otlicense

#pragma once

// OpenTwin header
#include "OTCore/JSON/JSON.h"
#include "OTCore/CoreAPIExport.h"

// std header
#include <list>
#include <string>

namespace ot {

	class OT_CORE_API_EXPORT JSONVectoriser
	{
	public:
		enum VectorisationFlag
		{
			NoFlags       = 0 << 0,
			AddLeavesOnly = 1 << 0,
			TopLevelOnly  = 1 << 1,

			Default = NoFlags
		};
		typedef Flags<VectorisationFlag> VectorisationFlags;

		static void vectorise(const JsonValue& _value, std::list<std::string>& _allEntries, const std::string& _nameBase, const VectorisationFlags& _flags = VectorisationFlag::Default);
		static const JsonValue& getValue(const JsonDocument& _structure, const std::string& _fieldName);

	private:
		static const std::string m_separator;
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::JSONVectoriser::VectorisationFlag, ot::JSONVectoriser::VectorisationFlags)