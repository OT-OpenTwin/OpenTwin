import json
import OpenTwin

class DataSet:
    def __init__(_self, _selectedCampaignAsJsonDocument):
        _self.m_allSeries = _selectedCampaignAsJsonDocument["series"]
    
    def getEmptyQuantityDescription(_self):
        empty = {
            "Label": "",
            "Name": "",
            "Dimensions": [1],
            "ValueDescriptions": [
                {
                    "Label": "",
                    "Name": "",
                    "Type": "",
                    "Unit": ""
                }
            ],
            "DependingParametersLabels": [""]
        }
    
        return empty

    def getEmptyParameterDescription(_self):
        empty = {
            "Label": "",
            "Name": "",
            "Type": "",
            "Unit": "",
            "Values": []
        }
        
        return empty
    
    def initiateNewSeries(_self, _seriesName):
        _self.m_newSeries = {
            "Label": "",
            "Name": _seriesName,
            "quantities": [],
            "parameter": []
        }
        _self.m_data =[]

    def addQuantityDescription(_self,_quantityDescription):
        _self.m_newSeries["quantities"].append(_quantityDescription)

    def addParameterDescription(_self,_parameterDescription):
        _self.m_newSeries["parameter"].append(_parameterDescription)

    def builtNewSeries(_self):
        _self.m_allSeries.append(_self.new_series)

    def addDataPoint(_self ,_quantityName, _quantityValue, **_parameterKeyValuePairs):
        entry={
            _quantityName : _quantityValue
        }
        for key, value in _parameterKeyValuePairs.items():
            entry[key] = _quantityValue
        _self.m_data.append(entry)        

dummyDataBase = {
    "Selected Campaign":{
	"Name":"Dataset/Campaign Metadata",
	"series":[
		{"Label":"",
		"Name":"Dataset/SingleCurve",
		"quantities":[
			{"Label":"Magnitude",
			"Name":"Magnitude",
			"Dimensions":[1],
			"ValueDescriptions":[
				{"Label":"",
				"Name":"",
				"Type":"int32",
				"Unit":"dB"}],
			"DependingParametersLabels":[
			"Frequency"]
			}
		],
		"parameter":[
			{"Label":"Frequency",
			"Name":"Frequency",
			"Type":"float",
			"Unit":"kHz",
			"Values":[0.0,1.0,2.0,3.0,4.0,5.0,6.0,7.0,8.0,9.0,10.0,11.0,12.0,13.0,14.0,15.0,16.0,17.0,18.0,19.0,20.0,21.0,22.0,23.0,24.0,25.0,26.0,27.0,28.0,29.0,30.0,31.0,32.0,33.0,34.0,35.0,36.0,37.0,38.0,39.0,40.0,41.0,42.0,43.0,44.0,45.0,46.0,47.0,48.0,49.0,50.0]}]}]}}
			
OpenTwin.Flush()
dataset = DataSet(dummyDataBase["Selected Campaign"])
newParameter = dataset.getEmptyParameterDescription()
newParameter["Name"] = "Epoche"
newParameter["Type"] = "int32"

newQuantity = dataset.getEmptyQuantityDescription()
newQuantity["Name"] = "R2"
newQuantity["ValueDescriptions"][0]["Type"] = "float"

dataset.initiateNewSeries("Model evaluations")



r2Values = [0.9, 0.84, 0.79, 0.75, 0.72, 0.66, 0.5]
epoches =[1,2,3,4,5,6,7]
anotherParameter =[4.1, 4.2, 4.3, 4.4, 4.5, 4.6 ,4.7]
for i in range(0,len(r2Values)):
    dataset.addDataPoint("R2",r2Values[i],Epoche=epoches[i], Something_else= anotherParameter[i])

dataset.addParameterDescription(newParameter)
dataset.addQuantityDescription(newQuantity)
dataset.builtNewSeries()

print(dummyDataBase)


