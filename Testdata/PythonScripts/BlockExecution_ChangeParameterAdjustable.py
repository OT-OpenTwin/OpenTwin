#@ Port : {"type": "in", "name": "DataInput", "label" : "Data Input"}
#@ Port : {"type": "out", "name": "DataOutput", "label" : " Data Output"}
#@ Property : {"label" : "Parameter name","type" : "string", "default" : ""}
#@ Property : {"label" : "New unit","type" : "string", "default" : ""}
import OpenTwin 
import json

def __main__(this):
    portData = OpenTwin.GetPortData("DataInput")
    portMetadata = OpenTwin.GetPortMetaData("DataInput")
    parameterName = OpenTwin.GetPropertyValue(this,"Parameter name")
    newUnit = OpenTwin.GetPropertyValue(this,"New unit")

    portMetadataObjects = json.loads(portMetadata)
    allSeries = portMetadataObjects["Selected Campaign"]["series"]
    for series in allSeries:
        parameterList = series["parameter"]
        for parameter in parameterList:
            if parameter["Label"] == parameterName:
                parameter["Unit"] = newUnit
                break
    
    print("Done")
    
    OpenTwin.SetPortData("DataOutput",portData)
    OpenTwin.SetPortMetaData("DataOutput",json.dumps(portMetadataObjects))
    
    
