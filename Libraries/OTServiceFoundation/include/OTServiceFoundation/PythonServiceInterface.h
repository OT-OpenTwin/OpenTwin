/*****************************************************************//**
 * \file   PythonServiceInterface.h
 * \brief  Class that serves as interface to the PythonExecutionService. Here a execution order can be assambled and send out to the pythonservice. 
 * 
 * \author Wagner
 * \date   August 2023
 *********************************************************************/
#pragma once
#include <string>
#include <tuple>
#include <optional>
#include <map>
#include <memory>

#include "OTCore/GenericDataStruct.h"
#include "OTCore/Variable.h"
#include "OTCore/ReturnMessage.h"
#include "OTServiceFoundation/FoundationAPIExport.h"

namespace ot
{
	class OT_SERVICEFOUNDATION_API_EXPORT PythonServiceInterface
	{
	public:

		using scriptParameter = std::optional<std::list<ot::Variable>>;
		PythonServiceInterface(const std::string& _pythonExecutionServiceURL);
		void addScriptWithParameter(const std::string& _scriptName, const scriptParameter& _scriptParameter);
		void addPortData(const std::string& _portName, const ot::JsonValue*  _data, const JsonValue* _metadata);
		
		ot::ReturnMessage sendExecutionOrder();

		ot::ReturnMessage sendSingleExecutionCommand(const std::string& command);

	private:
		const std::string m_pythonExecutionServiceURL;
		std::list<std::tuple<std::string, scriptParameter>> m_scriptNamesWithParameter;

		std::map<std::string, std::pair<const ot::JsonValue*, const ot::JsonValue*>> m_portDataByPortName;
		ot::JsonDocument assembleMessage();
	};
}
