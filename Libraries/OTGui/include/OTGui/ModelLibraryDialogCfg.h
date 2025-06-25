//! @file ModelLibraryDialogCfg.h
//! @author Sebastian Urmann	
//! @date June 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTGui/DialogCfg.h"
#include "OTGui/LibraryModel.h"

// std header
#include <string>

namespace ot {

	class OT_GUI_API_EXPORT ModelLibraryDialogCfg : public DialogCfg {
			OT_DECL_DEFCOPY(ModelLibraryDialogCfg)
			OT_DECL_DEFMOVE(ModelLibraryDialogCfg)
	public:
		ModelLibraryDialogCfg() = default;
		ModelLibraryDialogCfg(const std::list<LibraryModel>& _models,const std::list<std::string> _filter);
		virtual ~ModelLibraryDialogCfg() = default;

		// ###########################################################################################################################################################################################################################################################################################################################

		// Setter / Getter

		void addModel(const LibraryModel& _model);
		void setModels(const std::list<LibraryModel>& _models) { m_models = _models; };
		const std::list<LibraryModel>& getModels() const { return m_models; };

		void addFilter(const std::string& _filter);
		void setFilters(const std::list<std::string>& _filters) { m_filter = _filters; };
		const std::list<std::string>& getFilters() const { return m_filter; };

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
		std::list<LibraryModel> m_models;
		std::list<std::string> m_filter;
		
	};
}