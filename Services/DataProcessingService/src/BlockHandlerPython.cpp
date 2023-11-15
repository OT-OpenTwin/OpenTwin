#include "BlockHandlerPython.h"
#include "EntityProperties.h"
#include "Application.h"
#include "ClassFactory.h"
#include "EntityFile.h"

BlockHandlerPython::BlockHandlerPython(EntityBlockPython* blockEntity, const HandlerMap& handlerMap)
    : BlockHandler(handlerMap)
{
      ot::ServiceBase* pythonService = Application::instance()->getConnectedServiceByName(OT_INFO_SERVICE_TYPE_PYTHON_EXECUTION_SERVICE);
      const std::string pythonServiceURL = pythonService->serviceURL();
      _pythonServiceInterface = new ot::PythonServiceInterface(pythonServiceURL);

      auto allConnectorsByName = blockEntity->getAllConnectorsByName();
      for (auto& connectorByName : allConnectorsByName)
      {
           ot::Connector& connector = connectorByName.second;
           if (connector.getConnectorType() == ot::ConnectorType::In)
           {
               _requiredInput.push_back(connector.getConnectorName());
           }
      }     
      _entityName = blockEntity->getName();
      _scriptName = blockEntity->getSelectedScript();
}

bool BlockHandlerPython::executeSpecialized()
{
    bool allInputsComplete = true;
    for (std::string& requiredPort : _requiredInput)
    {
        allInputsComplete &= _dataPerPort.find(requiredPort) != _dataPerPort.end();
        if (!allInputsComplete) { break; }
    }
    
    if (allInputsComplete)
    {
        ot::PythonServiceInterface::scriptParameter parameter{ {ot::Variable(_entityName)} };
        _pythonServiceInterface->AddScriptWithParameter(_scriptName, parameter);
        for (auto& dataPortEntry : _dataPerPort)
        {
            const std::string portName = dataPortEntry.first;
            const genericDataBlock& portData = dataPortEntry.second;
            _pythonServiceInterface->AddPortData(portName, portData);
        }
        ot::ReturnMessage returnMessage = _pythonServiceInterface->SendExecutionOrder();
        if (returnMessage.getStatus() == ot::ReturnMessage::ReturnMessageStatus::Ok)
        {
            const std::string returnValue = returnMessage.getWhat();
            OT_rJSON_doc returnDoc = ot::rJSON::fromJSON(returnValue);
            if (ot::rJSON::memberExists(returnDoc, OT_ACTION_CMD_PYTHON_EXECUTE_RESULTS))
            {

            }
            else
            {
                //_uiComponent->displayMessage("Python script was executed succeblock did not receive any result da")
            }
        }
        else
        {
            _uiComponent->displayMessage(returnMessage.getWhat());
        }
    }
    return allInputsComplete;
}

