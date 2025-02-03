#pragma once
#include "BlockHandler.h"
#include "EntityBlockPlot1D.h"
#include "OTCore/FolderNames.h"

class BlockHandlerPlot1D : public BlockHandler
{
public:
	BlockHandlerPlot1D(EntityBlockPlot1D* blockEntity, const HandlerMap& handlerMap);
	virtual bool executeSpecialized() override;

private:
	std::string m_xDataConnector;
	std::list<std::string> m_yDataConnectors;
	std::list<std::string> m_curveNames;

	std::string m_resultFolder = ot::FolderNames::ResultFolder + "/";
	std::string m_plotName;
	std::string m_curveName = "Curve";
	const std::string m_curveFolderPath = m_resultFolder + "1D/Curves";
	std::string m_xlabel;
	std::string m_xunit; 
	std::string m_ylabel;
	std::string m_yunit;

	std::vector<double> loadXAxis();
	std::vector<double> transforParameterToDouble(const PipelineDataDocumentList& _documents);

	std::list<std::shared_ptr<EntityResult1DCurve>> createCurves(const PipelineData& _yAxis, const std::vector<double>& _xAxisValues, const std::string& _curveNameBase, int _colourID);

	void storeCurves(std::list<std::shared_ptr<EntityResult1DCurve>>& _curves);

	void setLabel(PipelineData& _pipelineData, std::string& _label);
	void setUnit(PipelineData& _pipelineData, std::string& _unit);

	std::map<ot::Variable, std::list<double>> transformQuantityToDouble(const PipelineDataDocumentList& _documents, const std::string& _group);
	double variableToDouble(const ot::Variable& var);
	const std::string getGroupingParameter(const PipelineData& _pipelineData);
	bool isAFamilyOfCurves(const PipelineDataDocumentList& _documents);
};
