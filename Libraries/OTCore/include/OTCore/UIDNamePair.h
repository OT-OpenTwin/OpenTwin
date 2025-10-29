// @otlicense

//! @file UIDNamePair.h
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Serializable.h"

// std header
#include <list>
#include <string>

namespace ot {
	
	class OT_CORE_API_EXPORT UIDNamePair : public Serializable {
	public:
		UIDNamePair();
		UIDNamePair(UID _uid, const std::string& _name);
		UIDNamePair(const ConstJsonObject& _jsonObject);
		UIDNamePair(const UIDNamePair& _other);
		virtual ~UIDNamePair();

		UIDNamePair& operator = (const UIDNamePair& _other);

		bool operator == (const UIDNamePair& _other) const;
		bool operator != (const UIDNamePair& _other) const;

		//! @brief Add the object contents to the provided JSON object.
		//! @param _document The JSON document (used to get the allocator).
		//! @param _object The JSON object to add the contents to.
		virtual void addToJsonObject(JsonValue& _object, JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object.
		//! @param _object The JSON object containing the information.
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types).
		virtual void setFromJsonObject(const ConstJsonObject& _object) override;

		void setUid(ot::UID _uid) { m_uid = _uid; };
		ot::UID getUid(void) const { return m_uid; };

		void setName(const std::string& _name) { m_name = _name; };
		const std::string& getName(void) const { return m_name; };

	private:
		UID m_uid;
		std::string m_name;
	};

	typedef std::list<ot::UIDNamePair> UIDNamePairList;

}