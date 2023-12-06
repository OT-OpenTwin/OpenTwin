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
	const std::string& getErrorReport() const { return _report; };
private:
	std::list<ot::JsonDocument *> _allConnectorsAsJSON;
	std::list<ot::JsonDocument*> _allPropertiesAsJSON;

	std::list<EntityPropertiesBase*> _allProperties;
	std::list<ot::Connector> _allConnectors;
	std::map<ot::JsonDocument *, int> _jsonEntryToScriptLine;
	std::string _report;

	const std::string _entryTypeNameProperty = "property";
	const std::string _entryTypeNameConnector = "port";

	const std::string _connectorDefTypeIn = "in";
	const std::string _connectorDefTypeOut = "out";
	const std::string _connectorDefTypeInOptional = "inopt";
	const std::string _connectorDefName = "name";

	const std::string _defType = "type";
	const std::string _defTitle = "label";

	const std::string _propertyDefTypeSelection = "selection";
	const std::string _propertyDefTypeString = "string";
	const std::string _propertyDefTypeDouble = "double";
	const std::string _propertyDefTypeInteger = "integer";
	const std::string _propertyDefTypeBoolean = "boolean";

	const std::string _propertyDefOptions = "options";
	const std::string _propertyDefDefault = "default";

	const std::string _propertyGroupName = "Script based";



	const std::string extractType(const std::string& lineContent);
	bool ExtractOTHeader(const std::string& scriptLine, const int scriptLineNumber);
	bool CreateObjectsFromJSON();
	bool CreateConnectorsFromJSON();
	bool CreatePropertiesFromJSON();
	ot::ConnectorType getConnectorType(ot::JsonDocument& jsonEntry, std::string& returnMessage);
	EntityPropertiesBase* createPropertyEntity(ot::JsonDocument& jsonEntry, std::string& returnMessage);
};