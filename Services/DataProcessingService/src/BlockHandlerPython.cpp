// @otlicense

#include "BlockHandlerPython.h"
#include "EntityProperties.h"
#include "Application.h"
#include "EntityFile.h"
#include "OTCore/ReturnValues.h"
#include "OTCommunication/ActionTypes.h"
#include "OTServiceFoundation/PythonServiceInterface.h"
#include "OTCore/GenericDataStructSingle.h"

BlockHandlerPython::BlockHandlerPython(EntityBlockPython* _blockEntity, const HandlerMap& _handlerMap)
    : BlockHandler(_blockEntity, _handlerMap)
{
      ot::ServiceBase* pythonService = Application::instance()->getConnectedServiceByName(OT_INFO_SERVICE_TYPE_PYTHON_EXECUTION_SERVICE);
      const std::string pythonServiceURL = pythonService->getServiceURL();
      m_pythonServiceInterface = new ot::PythonServiceInterface(pythonServiceURL);

      auto allConnectorsByName = _blockEntity->getAllConnectorsByName();
      for (auto& connectorByName : allConnectorsByName)
      {
           ot::Connector& connector = connectorByName.second;
           if (connector.getConnectorType() == ot::ConnectorType::In)
           {
               m_requiredInput.push_back(connector.getConnectorName());
           }
           else if (connector.getConnectorType() == ot::ConnectorType::Out)
           {
               m_outputs.push_back(connector.getConnectorName());
           }
      }     
      m_entityName = _blockEntity->getName();
      m_scriptName = _blockEntity->getSelectedScript();
}

bool BlockHandlerPython::executeSpecialized()
{
    //The block has n-ports which are defiend in the script. We need to check if all inputs are set.
    bool allInputsComplete = true;
    for (std::string& requiredPort : m_requiredInput)
    {
        allInputsComplete &= m_dataPerPort.find(requiredPort) != m_dataPerPort.end();
        if (!allInputsComplete) 
        { 
            break; 
        }
    }
    
    if (allInputsComplete)
    {
                
        //First assemble the job for the python service
        ot::PythonServiceInterface::scriptParameter parameter{ {ot::Variable(m_entityName)} };
        m_pythonServiceInterface->addScriptWithParameter(m_scriptName, parameter);
        for (auto& dataPortEntry : m_dataPerPort)
        {
            const std::string portName = dataPortEntry.first;
            PipelineData* incommingPortData  = dataPortEntry.second;
            if (incommingPortData != nullptr)
            {
                const ot::JsonValue& portData = incommingPortData->getData(); 
                const ot::JsonValue& portMetadata   = incommingPortData->getMetadata();
                m_pythonServiceInterface->addPortData(portName, &portData, &portMetadata);
            }
            else
            {
				SolverReport::instance().addToContentAndDisplay(getErrorDataPipelineNllptr() + " Port: " + portName + ".\n", _uiComponent);
            }
        }
        
        //Send the job
        ot::ReturnMessage returnMessage = m_pythonServiceInterface->sendExecutionOrder();

        //Post processing
        if (returnMessage.getStatus() == ot::ReturnMessage::ReturnMessageStatus::Ok)
        {
			SolverReport::instance().addToContentAndDisplay("Python script executed with state: " + returnMessage.getStatusString() + ".\n", _uiComponent);
            ot::ReturnValues& returnValues = returnMessage.getValues();
            ot::JsonDocument& values = returnValues.getValues();

            for (auto valueIt = values.MemberBegin(); valueIt != values.MemberEnd();valueIt++)
            {
                //Here we have JsonObjects for the different ports set
                const std::string portName = valueIt->name.GetString();

                auto output = std::find(m_outputs.begin(), m_outputs.end(), portName);
                if (output != m_outputs.end())
                {
                    auto& portValues = valueIt->value;
                    PipelineData pipelineData;
                    
                    if(portValues.HasMember("Data") == false || portValues.HasMember("Meta") == false)
                    {
                        throw std::exception(("Output port " + portName + " is missing 'Data' or 'Meta' fields.").c_str());
                        
					}
                    pipelineData.setData(std::move(portValues["Data"]));
                    const std::string tt = ot::json::toJson(portValues["Meta"]);
                    pipelineData.setMetadata(std::move(portValues["Meta"]));
                    m_outputData.push_back(std::move(pipelineData));
                    m_dataPerPort[portName] = &(m_outputData.back());
                    m_outputs.remove(portName);
                }
                else if (portName == m_scriptName)
                {
                    //Here we have the return value of the script
                    auto& portValues = valueIt->value;
					SolverReport::instance().addToContentAndDisplay("Python script returned message: " + ot::json::toJson(portValues) + "\n", _uiComponent);
                }
                else 
                {
					SolverReport::instance().addToContentAndDisplay("Port name used in python script does not match the listed ports: " + portName + ".\n", _uiComponent);
                }
            }

            //Check if all outputs were set in the script
            if (m_outputs.size() > 0)
            {
                for (const std::string& output : m_outputs)
                {
					SolverReport::instance().addToContentAndDisplay("Output port was not set in python script: " + output + "\n", _uiComponent);
                }
            }
        }
        else
        {
            throw std::exception(returnMessage.getWhat().c_str());
        }
    }
    return allInputsComplete;
}

std::string BlockHandlerPython::getBlockType() const
{
    return "Python Block";
}

