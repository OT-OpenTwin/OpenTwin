#pragma once
#include "OTCore/JSON.h"
#include "EntityFile.h"
#include "Connector.h"
#include <vector>
class PythonHeaderInterpreter
{
public:
	virtual ~PythonHeaderInterpreter();

	bool interprete(std::shared_ptr<EntityFile> pythonScript);
	const std::list<ot::Connector>& getAllConnectors() const { return _allConnectors; };
	const std::list<EntityPropertiesBase*>& getAllProperties()const { return _allProperties; };

private:
	std::list<ot::JsonDocument *> _allConnectorsAsJSON;
	std::list<ot::JsonDocument*> _allPropertiesAsJSON;

	std::list<EntityPropertiesBase*> _allProperties;
	std::list<ot::Connector> _allConnectors;

	std::string _report;

	const std::vector<std::string> _entryTypeNamesProperty{"Property", "property", "PROPERTY"};
	const std::vector<std::string> _entryTypeNamesConnector{"Port", "port", "PORT"};

	const std::vector<std::string> _connectorDefType{ "Type","type", "TYPE" };
	const std::vector<std::string> _connectorDefTypeIn{ "In", "in", "IN" };
	const std::vector<std::string> _connectorDefTypeOut{ "Out", "out", "OUT" };
	const std::vector<std::string> _connectorDefTypeInOptional{ "InOpt", "inopt", "INOPT","Inopt", "in_opt", "In_Opt","In_opt" };
	const std::vector<std::string> _connectorDefName{ "Name", "name", "NAME" };
	const std::vector<std::string> _connectorDefTitle{ "Title", "title", "TITLE" };

	bool ExtractOTHeader(const std::string& scriptLine);
	bool CreateObjectsFromJSON();
	ot::ConnectorType getConnectorType(const ot::ConstJsonObject& jsonEntry);
};