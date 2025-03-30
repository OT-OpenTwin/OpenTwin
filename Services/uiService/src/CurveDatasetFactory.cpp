#include "CurveDatasetFactory.h"
#include "ContainerFlexibleOwnership.h"

std::list<ot::PlotDataset*> CurveDatasetFactory::createCurves(const ot::Plot1DCurveCfg& _config)
{
	std::list<ot::PlotDataset*> dataSets;

	auto queryInformation = _config.getQueryInformation();

	DataStorageAPI::DataStorageResponse dbResponse = m_dataAccess.SearchInResultCollection(queryInformation.m_query, queryInformation.m_projection, 0);
	if (dbResponse.getSuccess()) {
		const std::string queryResponse = dbResponse.getResult();
		ot::JsonDocument doc;
		doc.fromJson(queryResponse);
		auto allMongoDocuments = ot::json::getArray(doc, "Documents");
		const uint32_t numberOfDocuments = allMongoDocuments.Size();

		std::unique_ptr<double[]> dataY(new double[numberOfDocuments]);
		std::unique_ptr<double[]> dataYim(new double[numberOfDocuments]);
		size_t counter(0);

		for (uint32_t i = 0; i < numberOfDocuments; i++) {
			auto singleMongoDocument = ot::json::getObject(allMongoDocuments, i);
			auto& quantityEntry = singleMongoDocument[queryInformation.m_quantityDescription.m_fieldName.c_str()];
			int32_t quantityValue = quantityEntry.GetInt();
			(dataY)[counter] = static_cast<double>(quantityValue);
			counter++;
		}

		size_t numberOfParameter = queryInformation.m_parameterDescriptions.size();
		if (numberOfParameter == 1) {
			std::unique_ptr<double[]> dataX(new double[numberOfDocuments]);
			counter = 0;
			//Single curve
			auto entryDescription = queryInformation.m_parameterDescriptions.begin();
			for (uint32_t i = 0; i < numberOfDocuments; i++) {
				auto singleMongoDocument = ot::json::getObject(allMongoDocuments, i);
				auto& parameterEntry = singleMongoDocument[entryDescription->m_fieldName.c_str()];
				int32_t parameterValue = parameterEntry.GetInt();
				(dataX)[counter] = static_cast<double>(parameterValue);
				counter++;
			}

			double* xData = dataX.release();
			double* yData = dataY.release();
			double* yimData = dataYim.release();
			dataSets.push_back(new ot::PlotDataset(nullptr, _config, ot::PlotDatasetData(xData, nullptr, yData, yimData, nullptr, counter)));
		}
		else {
			//Lables of curves will be <parameter_Label_1>_<parameter_Value_1>_<parameter_Unit_1>_ ... _<parameter_Label_n>_<parameter_Value_n>_<parameter_Unit_n>
			//Should be selected via a property ToDo
			auto xAxisParameter = queryInformation.m_parameterDescriptions.begin();

			std::map<std::string, ContainerFlexibleOwnership<double>> familyOfCurves;
			std::map<std::string, std::list<AdditionalParameterDescription>> additionalParameterDescByCurveName;
			for (uint32_t i = 0; i < numberOfDocuments; i++) {
				auto singleMongoDocument = ot::json::getObject(allMongoDocuments, i);

				//First build a unique name of the additional parameter values
				std::string curveName("");
				std::list<AdditionalParameterDescription> additionalParameterInfos;
				for (auto additionalParameter = queryInformation.m_parameterDescriptions.begin()++; additionalParameter != queryInformation.m_parameterDescriptions.end(); additionalParameter++) {
					auto& additionalParameterEntry = singleMongoDocument[additionalParameter->m_fieldName.c_str()];
					curveName = additionalParameter->m_label + "_" + ot::json::toJson(additionalParameterEntry) + "_" + additionalParameter->m_unit + "; ";

					AdditionalParameterDescription additionalParameterInfo;
					additionalParameterInfo.m_label = additionalParameter->m_label;
					additionalParameterInfo.m_value = ot::json::toJson(additionalParameterEntry);
					additionalParameterInfo.m_unit = additionalParameter->m_unit;
					additionalParameterInfos.push_back(additionalParameterInfo);
				}
				curveName = curveName.substr(0, curveName.size() - 2);

				auto curve = familyOfCurves.find(curveName);
				if (curve == familyOfCurves.end()) {
					familyOfCurves.insert({ curveName, ContainerFlexibleOwnership<double>(numberOfDocuments) });
					additionalParameterDescByCurveName[curveName] = additionalParameterInfos;
					curve = familyOfCurves.find(curveName);
				}

				auto& xAxisParameterEntry = singleMongoDocument[xAxisParameter->m_fieldName.c_str()];
				(curve->second).pushBack(static_cast<double>(xAxisParameterEntry.GetInt()));
			}

			//In this case we need to make the names better readable. Since we have more then one parameter in the name 
			if (numberOfParameter > 2) {
				std::map<std::string, ContainerFlexibleOwnership<double>> familyOfCurvesSimplerNames;
				std::list <std::string> runIDDescriptions;

				int counter(0);
				for (auto curve : familyOfCurves) {
					const std::string simpleName = "RunID_" + std::to_string(counter);
					familyOfCurvesSimplerNames.insert({ simpleName,curve.second });
					counter++;
					std::list<AdditionalParameterDescription>& additionalParameterDescription = additionalParameterDescByCurveName[curve.first];
					std::string message =
						simpleName + ":\n";
					for (auto entry : additionalParameterDescription) {
						message += "	" + entry.m_label + " = " + entry.m_value + " " + entry.m_unit + "\n";
					}
					//AppBase::instance()->appendInfoMessage(QString::fromStdString(message));
				}

				familyOfCurves = familyOfCurvesSimplerNames;
			}

			for (auto singleCurve : familyOfCurves) {
				ot::Plot1DCurveCfg singleCurveCfg = _config;
				//singleCurveCfg.setXAxisTitle();
				//singleCurveCfg.setXAxisUnit();
				// Must be possible to set a label here. Seems that currently it is only possible to use the entity name for this purpose. 
				// Could there be an issue that multiple curves are associated with a single curve entity ? Changes would always affect all curves in the family.
				// Probably iterate the colour of each curve here
				ot::Color colour = singleCurveCfg.getLinePenColor();

				double* xData = singleCurve.second.release();
				double* yData = dataY.release();
				/*
				std::string dbg = "Adding cuve with points:";
				for (size_t i = 0; i < numberOfDocuments; i++) {
					dbg.append("\n[" + std::to_string(i) + "]: " + std::to_string(xData[i]) + "; " + std::to_string(yData[i]));
				}
				dbg.append("\n\nn = " + std::to_string(numberOfDocuments) + "\n\n");

				OT_LOG_W(dbg);
				*/
			}
		}
		//dataSet->updateCurveVisualization();
		//dataSet->attach();
	}
	return dataSets;
}
