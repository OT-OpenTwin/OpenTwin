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
	void sanitiseKeys(const ot::JsonValue& _src, ot::JsonValue& _dest, ot::JsonAllocator& _alloc);
};
