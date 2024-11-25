//! @file StyledTextBuilder.h
//! @author Alexander Kuester (alexk95)
//! @date November 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/StyledTextEntry.h"

// std header
#include <list>

namespace ot {

	class OT_GUI_API_EXPORT StyledTextBuilder : public Serializable {
	public:
		StyledTextBuilder();
		StyledTextBuilder(const ConstJsonObject& _jsonObject);
		StyledTextBuilder(const StyledTextBuilder& _other) = default;
		virtual ~StyledTextBuilder() = default;
		StyledTextBuilder& operator = (const StyledTextBuilder& _other) = default;

		//! \brief Add the object contents to the provided JSON object.
		//! \param _object Json object reference to write the data to.
		//! \param _allocator Allocator.
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! \brief Set the object contents from the provided JSON object.
		//! \param _object The JSON object containing the information.
		//! \throw May throw an exception if the provided object is not valid (members missing or invalid types).
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		//! @brief Returns true if the builder has no entries that should be displayed (no text provided).
		bool isEmpty(void) const;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Builder

		const std::list<StyledTextEntry> getEntries(void) const { return m_entries; };

		StyledTextBuilder& operator << (const char* _text);
		StyledTextBuilder& operator << (const std::string& _text);

		StyledTextBuilder& operator << (StyledText::ColorReference _colorReference);

		StyledTextBuilder& operator << (StyledText::TextControl _control);

	private:
		void applyNextStyle(const StyledTextStyle& _style);

		std::list<StyledTextEntry> m_entries;
	};

}