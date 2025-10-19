//! @file EntityFactory.h
//! @author Alexander Kuester (alexk95)
//! @date October 2025
// ###########################################################################################################################################################################################################################################################################################################################

#pragma once

// OpenTwin header
#include "OTCore/OTClassHelper.h"
#include "OTGui/FileExtension.h"
#include "ModelEntitiesAPIExport.h"

// std header
#include <map>
#include <string>
#include <functional>

class EntityBase;

//! @brief The EntityFactory class is a singleton class that allows to create instances of entity classes based on their class name.
class OT_MODELENTITIES_API_EXPORT EntityFactory {
	OT_DECL_NOCOPY(EntityFactory)
	OT_DECL_NOMOVE(EntityFactory)
public:
	typedef std::function<EntityBase*()> CreateEntityFunction;

	//! @brief This method returns the single global instance of the factory.
	//! The instance exists only once in the application's memory, ensuring that all libraries share the same instance.
	static EntityFactory& instance();

	//! @brief Creates an instance of the class with the given name.
	//! @param _className The name of the class to create an instance of.
	//! @return A pointer to the created instance, or nullptr if the class name is unknown.
	EntityBase* create(const std::string& _className);

	//! @brief Creates an instance of the class registered for the given file extension.
	//! @param _fileExtension File extension to create the class for.
	//! @return A pointer to the created instance, or nullptr if the file extension is unknown.
	EntityBase* create(ot::FileExtension::DefaultFileExtension _fileExtension);

	//! @brief Registers a class with the factory.
	//! @param _className The name of the class to register.
	//! @param _createFunction A function that creates an instance of the class.
	//! @return true if the class was registered successfully, false if the class name is already registered.
	bool registerClass(const std::string& _className, CreateEntityFunction _createFunction);

	//! @brief Registers a class with the factory for a specific file extension.
	//! @param _fileExtension File extension to register the class for.
	//! @param _createFunction A function that creates an instance of the class.
	//! @return true if the class was registered successfully, false if the file extension is already registered.
	bool registerClass(ot::FileExtension::DefaultFileExtension _fileExtension, CreateEntityFunction _createFunction);

private:
	EntityFactory() = default;
	~EntityFactory() = default;

	std::map<std::string, CreateEntityFunction> m_creators;
	std::map<ot::FileExtension::DefaultFileExtension, CreateEntityFunction> m_fileExtensionCreators;
};

