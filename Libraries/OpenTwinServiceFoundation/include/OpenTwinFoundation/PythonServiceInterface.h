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
		ot::ReturnMessage SendExecutionOrder();

	private:
		const std::string _pythonExecutionServiceURL;
		std::list<std::tuple<std::string, scriptParameter>> _scriptNamesWithParameter;

		OT_rJSON_doc AssembleMessage();
	};
}
