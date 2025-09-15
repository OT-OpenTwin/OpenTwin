//! @file OnePropertyDialogCfg.h
//! @author Alexander Kuester (alexk95)
//! @date February 2024
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/DialogCfg.h"

namespace ot {

	class Property;

	class OT_GUI_API_EXPORT OnePropertyDialogCfg : public DialogCfg {
		OT_DECL_NOCOPY(OnePropertyDialogCfg)
		OT_DECL_NOMOVE(OnePropertyDialogCfg)
	public:
		OnePropertyDialogCfg(Property* _property = (Property*)nullptr);
		OnePropertyDialogCfg(const std::string& _title, Property* _property = (Property*)nullptr);
		virtual ~OnePropertyDialogCfg();

		//! @brief Add the object contents to the provided JSON object.
		//! Will ignore the PropertyBase::NoSerialize flag.
		//! @param _object Json object reference.
		//! @param _allocator Allocator.
		virtual void addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const override;

		//! @brief Will set the object contents from the provided JSON object.
		//! @param _object The JSON object containing the information.
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types).
		virtual void setFromJsonObject(const ot::ConstJsonObject& _object) override;

		//! @brief Set or replace the property
		//! If a property was set before it will be destroyed
		void setProperty(Property* _property);
		Property* getProperty(void) const { return m_property; };

		//! @brief Returns the current property and replaces it with null
		Property* takeProperty(void);

	private:
		Property* m_property;
	};

}