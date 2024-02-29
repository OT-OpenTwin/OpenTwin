//! @file DialogCfg.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/Flags.h"
#include "OTCore/Serializable.h"
#include "OTCore/OTClassHelper.h"
#include "OTGui/OTGuiAPIExport.h"

// std header
#include <string>

#pragma warning(disable:4251)

namespace ot {

	class OT_GUI_API_EXPORT DialogCfg : public Serializable {
		OT_DECL_NOCOPY(DialogCfg)
	public:
		enum DialogFlags {
			NoFlags          = 0x00, //! @brief No dialog flags
			CancelOnNoChange = 0x01 //! @brief If set the UI will silently close the dialog when pressing confim button and the initial data did not change
		};

		static std::string flagToString(DialogFlags _flag);
		static DialogFlags stringToFlag(const std::string& _flag);
		static std::list<std::string> flagsToStringList(DialogFlags _flags);
		static DialogFlags stringListToFlags(const std::list<std::string>& _flags);

		DialogCfg(DialogFlags _flags = DialogFlags::NoFlags) : m_flags(_flags) {};
		DialogCfg(const std::string& _title, DialogFlags _flags = DialogFlags::NoFlags) : m_flags(_flags), m_title(_title) {};
		virtual ~DialogCfg() {};

		//! @brief Add the object contents to the provided JSON object
		//! @param _object Json object reference
		//! @param _allocator Allocator
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		void setName(const std::string& _name) { m_name = _name; };
		const std::string& name(void) const { return m_name; };

		void setTitle(const std::string& _title) { m_title = _title; };
		const std::string title(void) const { return m_title; };

		void setFlags(DialogFlags _flags) { m_flags = _flags; };
		DialogFlags flags(void) const { return m_flags; };

	private:
		std::string m_name;
		std::string m_title;
		DialogFlags m_flags;
	};

}

OT_ADD_FLAG_FUNCTIONS(ot::DialogCfg::DialogFlags)