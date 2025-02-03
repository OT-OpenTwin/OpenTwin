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
      _pythonServiceInterface = new ot::PythonServiceInterface(pythonServiceURL);

      auto allConnectorsByName = blockEntity->getAllConnectorsByName();
      for (auto& connectorByName : allConnectorsByName)
      {
           ot::Connector& connector = connectorByName.second;
           if (connector.getConnectorType() == ot::ConnectorType::In)
           {
               _requiredInput.push_back(connector.getConnectorName());
           }
           else if (connector.getConnectorType() == ot::ConnectorType::Out)
           {
               _outputs.push_back(connector.getConnectorName());
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
        _uiComponent->displayMessage("Executing Python Block: " + _blockName);
        ot::PythonServiceInterface::scriptParameter parameter{ {ot::Variable(_entityName)} };
        _pythonServiceInterface->AddScriptWithParameter(_scriptName, parameter);
        for (auto& dataPortEntry : _dataPerPort)
        {
            const std::string portName = dataPortEntry.first;
            PipelineData& incommingPortData  = dataPortEntry.second;
            const PipelineDataDocumentList& portData = incommingPortData.m_data;
            
            std::list<ot::GenericDataStruct*> portDataPtr;
            for (auto portDataEntry : portData)
            {
                portDataPtr.push_back(portDataEntry.m_quantity.get());
            }

            _pythonServiceInterface->AddPortData(portName, portDataPtr);
        }
        ot::ReturnMessage returnMessage = _pythonServiceInterface->SendExecutionOrder();
        if (returnMessage.getStatus() == ot::ReturnMessage::ReturnMessageStatus::Ok)
        {
            _uiComponent->displayMessage("Python execution ended with state: " + returnMessage.getStatusString());
            ot::ReturnValues& returnValues = returnMessage.getValues();
            auto& valuesByName =  returnValues.getValuesByName();
            for (const std::string& outputName : _outputs)
            {
                auto valuesPointer = valuesByName.find(outputName);
                if (valuesPointer == valuesByName.end())
                {
                    _uiComponent->displayMessage("Output " + outputName + " was not set in the python script.");
                }
                else
                {
                    auto& returnValueList = valuesPointer->second;
                    PipelineDataDocumentList returnValuesListSPtr;
                    for (auto returnValue : returnValueList)
                    {
                        PipelineDataDocument document;
                        document.m_quantity = std::shared_ptr<ot::GenericDataStruct>(returnValue);
                        returnValuesListSPtr.push_back(document);
                    }
                    PipelineData outputData;
                    outputData.m_data = std::move(returnValuesListSPtr);
                    _dataPerPort[outputName] = outputData;
                    valuesByName.erase(valuesPointer->first);
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

