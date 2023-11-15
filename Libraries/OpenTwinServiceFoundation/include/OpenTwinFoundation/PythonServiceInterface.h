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

#include "OpenTwinCore/Variable.h"
#include "OpenTwinCore/ReturnMessage.h"
#include "OpenTwinFoundation/FoundationAPIExport.h"


namespace ot
{
	class OT_SERVICEFOUNDATION_API_EXPORT PythonServiceInterface
	{
	public:
		using scriptParameter = std::optional<std::list<ot::Variable>>;
		PythonServiceInterface(const std::string& pythonExecutionServiceURL);
		void AddScriptWithParameter(const std::string& scriptName, const scriptParameter& scriptParameter);
		void AddPortData(const std::string& portName, const std::list<ot::Variable>& data);

		ot::ReturnMessage SendExecutionOrder();

	private:
		const std::string _pythonExecutionServiceURL;
		std::list<std::tuple<std::string, scriptParameter>> _scriptNamesWithParameter;
		std::map<std::string, std::list<ot::Variable>> _portDataByPortName;
		OT_rJSON_doc AssembleMessage();
	};
}
