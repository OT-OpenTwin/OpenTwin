/*
 * FDTDPropertyReader.h
 *
 *  Created on: 15.10.2025
 *	Author: Alexandros McCray (alexm-dev)
 *  Copyright (c) 2025
 */

#pragma once

#include "EntityProperties.h"

class EntityBase;

//! @brief Template function to read entity properties of different types
//! @param T The type of the property to read (double, int, string, etc.)
//! @param solverEntity Pointer to the solver entity
//! @param groupFilter The group filter to apply when reading properties
//! @param properties Map to store the read properties
//! @param propertyName The name of the property to read
//! @param modelServiceAPICall Flag indicating whether to call the ModelServiceAPI to read properties
//! @return The value of the requested property
template<typename T>
T readEntityPropertiesInfo(EntityBase* _solverEntity, const std::string& _groupFilter, std::map<ot::UID, EntityProperties>& _properties, const std::string& _propertyName, bool _modelServiceAPICall);



#include "FDTDPropertyReader.hpp"