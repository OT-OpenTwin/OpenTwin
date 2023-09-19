//! @file GraphicsPackage.h
//! @author Alexander Kuester (alexk95)
//! @date August 2023
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTGui/OTGuiAPIExport.h"
#include "OpenTwinCore/Serializable.h"
#include "OpenTwinCore/SimpleFactory.h"

// std header
#include <list>

namespace ot {

	class GraphicsItemCfg;
	class GraphicsCollectionCfg;
	
	//! @brief Package that will be used to fill the graphics picker
	//! The name of the editor must be unique. If an editor with the given name already exists, the creation will fail.
	//! Note that the request to the UI must be send manually by providing this object (serialized) as the package
	class OT_GUI_API_EXPORT GraphicsCollectionPackage : public ot::Serializable {
	public:
		GraphicsCollectionPackage();
		virtual ~GraphicsCollectionPackage();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

		void addCollection(GraphicsCollectionCfg* _collection);
		const std::list<GraphicsCollectionCfg*>& collections(void) const { return m_collections; };

	private:
		void memFree(void);

		std::list<GraphicsCollectionCfg*> m_collections;

		GraphicsCollectionPackage(const GraphicsCollectionPackage&) = delete;
		GraphicsCollectionPackage& operator = (const GraphicsCollectionPackage&) = delete;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	//! @brief Package that will be used to create a new graphics editor
	//! The name of the editor must be unique. If an editor with the given name already exists, the creation will fail.
	//! Note that the request to the UI must be send manually by providing this object (serialized) as the package
	class OT_GUI_API_EXPORT GraphicsNewEditorPackage : public ot::GraphicsCollectionPackage {
	public:
		//! @brief Constructor
		//! @param _editorName The unique name for the editor.
		GraphicsNewEditorPackage(const std::string& _editorName, const std::string& _editorTitle);
		virtual ~GraphicsNewEditorPackage();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

		const std::string& name(void) const { return m_name; };
		const std::string& title(void) const { return m_title; };

	private:
		std::string m_name;
		std::string m_title;

		GraphicsNewEditorPackage() = delete;
		GraphicsNewEditorPackage(const GraphicsNewEditorPackage&) = delete;
		GraphicsNewEditorPackage& operator = (const GraphicsNewEditorPackage&) = delete;
	};

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	// ###########################################################################################################################################################################################################################################################################################################################

	//! @brief Package that will be used to add graphics items to an existing scene.
	//! Note that the request to the UI must be send manually by providing this object (serialized) as the package
	class OT_GUI_API_EXPORT GraphicsScenePackage : public ot::Serializable {
	public:
		GraphicsScenePackage(const std::string& _editorName);
		virtual ~GraphicsScenePackage();

		//! @brief Add the object contents to the provided JSON object
		//! @param _document The JSON document (used to get the allocator)
		//! @param _object The JSON object to add the contents to
		virtual void addToJsonObject(OT_rJSON_doc& _document, OT_rJSON_val& _object) const override;

		//! @brief Will set the object contents from the provided JSON object
		//! @param _object The JSON object containing the information
		//! @throw Will throw an exception if the provided object is not valid (members missing or invalid types)
		virtual void setFromJsonObject(OT_rJSON_val& _object) override;

		void addItem(GraphicsItemCfg* _item);
		const std::list<GraphicsItemCfg*>& items(void) const { return m_items; };

		void setName(const std::string& _name) { m_name = _name; };
		const std::string& name(void) const { return m_name; };

	private:
		void memFree(void);

		std::string m_name;
		std::list<ot::GraphicsItemCfg*> m_items;

	};

}