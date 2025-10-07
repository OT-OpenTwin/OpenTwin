#@ Port : {"type": "in", "name": "DataInput", "label" : "Data Input"}
#@ Port : {"type": "out", "name": "DataOutput", "label" : " Data Output"}

import OpenTwin 
import json

def __main__(this):
    portData = OpenTwin.GetPortData("DataInput")
    portMetadata = OpenTwin.GetPortMetaData("DataInput")
    
    portDataObjects = json.loads(portData)
    numberOfEntries = len(portDataObjects)

    print("Counted: " + str(numberOfEntries) + " entries, ready for machine learning")
    OpenTwin.SetPortData("DataOutput",json.dumps(portDataObjects))
    OpenTwin.SetPortMetaData("DataOutput",portMetadata)
    
    
