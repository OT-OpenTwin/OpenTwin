#pragma once
#include <string>
#include <map>
#include <list>
#include <MetadataParameter.h>
#include <MetadataQuantity.h>

class __declspec(dllexport) SeriesMetadata
{
public:
	SeriesMetadata(const std::string& name):_name(name) {};
	void AddParameter(MetadataParameter&& parameter) { _parameter.push_back(parameter); }
	void AddQuantity(MetadataQuantity&& quantity) { _quantity.push_back(quantity); }
	void AddMetadata(std::shared_ptr<MetadataEntry> metadata) { _metaData[metadata->getEntryName()] = metadata; }
	const std::list<MetadataParameter>& getParameter() const { return _parameter; }
	const std::list<MetadataQuantity>& getQuantities() const { return _quantity; }
private:
	const std::string _name;
	
	std::list<MetadataParameter> _parameter;
	std::list<MetadataQuantity> _quantity;
	
	std::map <std::string, std::shared_ptr<MetadataEntry>> _metaData;

};
