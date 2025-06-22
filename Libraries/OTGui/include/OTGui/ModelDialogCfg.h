//! @file ModelDialogCfg.h
//! @author Sebastian Urmann	
//! @date June 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/DialogCfg.h"
#include "OTCore/OTClassHelper.h"

// Service header
#include "LibraryModel.h"

// std header
#include <string>


namespace ot {

	class OT_GUI_API_EXPORT ModelDialogCfg : public DialogCfg {
			OT_DECL_DEFCOPY(ModelDialogCfg)
			OT_DECL_DEFMOVE(ModelDialogCfg)


	public:
		ModelDialogCfg() = default;
		ModelDialogCfg(const std::list<LibraryModel>& _models,const std::list<std::string> _filter);
		~ModelDialogCfg() = default;

		// model list functions

		void addModel(const LibraryModel& _model);
		void setModelList(const std::list<LibraryModel>& _models);
		const std::list<LibraryModel>& getModelList(void) const { return m_models; };

		// filter list functions

		void addFilter(const std::string& _filter);
		void setFilterList(const std::list<std::string>& _filters);
		const std::list<std::string>& getFilterList(void) const { return m_filter; };


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