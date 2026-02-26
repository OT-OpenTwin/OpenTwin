//@otlicense
#pragma once
#include "OTModelEntities/EntityMetadataSeries.h"
#include "OTModelEntities/EntityFileText.h"
#include "OTModelEntities/EntityInformation.h"

class MetadataExtender
{
public:
	void extendWithJsonFile();

private:
	void loadRelevantSelectedEntities(std::list<ot::EntityInformation>& selectedFiles, std::list<ot::EntityInformation>& selectedSeries);
	std::list<std::unique_ptr<EntityMetadataSeries>> getSelectedSeriesMetadata(std::list<ot::EntityInformation>& _selectedSeries);
	std::list < std::unique_ptr <EntityFileText>> getSelectedJsonFiles(std::list<ot::EntityInformation>& _selectedFiles);

	//! @brief Merges two objects recursively. Identical fields get their values overwritten by the _srcObject, except they are arrays or documents, which get extended.
	//! @param _allocator 
	//! @param _secureMerge If true, an exception is thrown if 
	void mergeObjects(rapidjson::Value& _dstObject, rapidjson::Value& _srcObject, rapidjson::Document::AllocatorType& _allocator, bool _secureMerge = true);
};
