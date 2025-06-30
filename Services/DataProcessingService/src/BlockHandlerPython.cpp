#include "BlockHandlerPython.h"
#include "EntityProperties.h"
#include "Application.h"
#include "ClassFactory.h"
#include "EntityFile.h"
#include "OTCore/ReturnValues.h"
#include "OTServiceFoundation/PythonServiceInterface.h"
#include "OTCore/GenericDataStructSingle.h"

BlockHandlerPython::BlockHandlerPython(EntityBlockPython* blockEntity, const HandlerMap& handlerMap)
    : BlockHandler(blockEntity, handlerMap)
{
      ot::ServiceBase* pythonService = Application::instance()->getConnectedServiceByName(OT_INFO_SERVICE_TYPE_PYTHON_EXECUTION_SERVICE);
      const std::string pythonServiceURL = pythonService->getServiceURL();
      m_pythonServiceInterface = new ot::PythonServiceInterface(pythonServiceURL);

      auto allConnectorsByName = blockEntity->getAllConnectorsByName();
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
      m_entityName = blockEntity->getName();
      m_scriptName = blockEntity->getSelectedScript();
}

bool BlockHandlerPython::executeSpecialized()
{
    bool allInputsComplete = true;
    for (std::string& requiredPort : m_requiredInput)
    {
        allInputsComplete &= _dataPerPort.find(requiredPort) != _dataPerPort.end();
        if (!allInputsComplete) { break; }
    }
    
    if (allInputsComplete)
    {
        _uiComponent->displayMessage("Executing Python Block: " + _blockName);
        ot::PythonServiceInterface::scriptParameter parameter{ {ot::Variable(m_entityName)} };
        m_pythonServiceInterface->addScriptWithParameter(m_scriptName, parameter);
        for (auto& dataPortEntry : _dataPerPort)
        {
            const std::string portName = dataPortEntry.first;
            PipelineData* incommingPortData  = dataPortEntry.second;
            const ot::JsonDocument& portData = incommingPortData->m_data;            
            m_pythonServiceInterface->addPortData(portName, &portData);
        }
        ot::ReturnMessage returnMessage = m_pythonServiceInterface->sendExecutionOrder();
        if (returnMessage.getStatus() == ot::ReturnMessage::ReturnMessageStatus::Ok)
        {
            _uiComponent->displayMessage("Python execution ended with state: " + returnMessage.getStatusString());
            ot::ReturnValues& returnValues = returnMessage.getValues();
            ot::JsonDocument& values = returnValues.getValues();

            for (auto valueIt = values.MemberBegin(); valueIt != values.MemberEnd();valueIt++)
            {
                const std::string portName = valueIt->name.GetString();
                
                auto output = std::find(m_outputs.begin(), m_outputs.end(), portName);
                if (output != m_outputs.end())
                {
                    auto& portValues = valueIt->value;
                    PipelineData pipelineData;
                    pipelineData.setData(std::move(portValues));
                    m_outputData.push_back(pipelineData);
                    m_outputs.remove(portName);
                }
                else
                {
                    _uiComponent->displayMessage("Port name used in python script does not match the listed ports: " + portName);
                }
            }

            if (m_outputs.size() > 0)
            {
                for (const std::string& output : m_outputs)
                {
                    _uiComponent->displayMessage("Port was not set in python script: " + output);
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

