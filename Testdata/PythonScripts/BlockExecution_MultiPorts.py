#@ Port : {"type": "in", "name": "DataInput", "label" : "Data Input"}
#@ Port : {"type": "out", "name": "NoOffset", "label" : "No Offset"}
#@ Port : {"type": "out", "name": "Offset", "label" : "Offset"}
#@ Property : {"label" : "Offset","type" : "integer", "default" : 2}
import OpenTwin 
import json

def __main__(this):
    portData = OpenTwin.GetPortData("DataInput")
    portMetadata = OpenTwin.GetPortMetaData("DataInput")
    offset = OpenTwin.GetPropertyValue(this,"Offset")
   
    portDataObjects = json.loads(portData)
    for data in portDataObjects:
        data["Magnitude"] = int(data["Magnitude"]) + offset
    print("Done")
    print(type(portDataObjects))
    OpenTwin.SetPortData("Offset",json.dumps(portDataObjects))
    OpenTwin.SetPortMetaData("Offset",portMetadata)
    
    OpenTwin.SetPortData("NoOffset",portData)
    OpenTwin.SetPortMetaData("NoOffset",portMetadata)
    
