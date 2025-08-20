#@ Port : {"type": "in", "name": "DataInput", "label" : "Data Input"}
#@ Port : {"type": "out", "name": "DataOutput", "label" : " Data Output"}

import OpenTwin 
import json

def __main__(this):
    portData = OpenTwin.GetPortData("DataInput")
    portMetadata = OpenTwin.GetPortMetaData("DataInput")
    portMetadataObjects = json.loads(portMetadata)
    allSeries = portMetadataObjects["Selected Campaign"]["series"]
    for series in allSeries:
        parameterList = series["parameter"]
        for parameter in parameterList:
            if parameter["Label"] == "Frequency":
                parameter["Unit"] = "Hz"
                break
    
    print("Done")
    
    OpenTwin.SetPortData("DataOutput",portData)
    OpenTwin.SetPortMetaData("DataOutput",json.dumps(portMetadataObjects))
    
    
