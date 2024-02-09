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
		using GenericDataList = std::list<std::shared_ptr<ot::GenericDataStruct>>;

		using scriptParameter = std::optional<std::list<ot::Variable>>;
		PythonServiceInterface(const std::string& pythonExecutionServiceURL);
		void AddScriptWithParameter(const std::string& scriptName, const scriptParameter& scriptParameter);
		void AddPortData(const std::string& portName, const GenericDataList& data);

		ot::ReturnMessage SendExecutionOrder();

		ot::ReturnMessage SendSingleExecutionCommand(const std::string& command);

	private:
		const std::string _pythonExecutionServiceURL;
		std::list<std::tuple<std::string, scriptParameter>> _scriptNamesWithParameter;

		std::map<std::string, GenericDataList> _portDataByPortName;
		ot::JsonDocument AssembleMessage();
	};
}
