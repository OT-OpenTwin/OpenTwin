#@ Port : {"type": "in", "name": "DataInput", "label" : "Data Input"}
#@ Port : {"type": "out", "name": "DataOutput", "label" : " Data Output"}
#@ Property : {"label" : "Offset","type" : "double", "default" : 2.0}
import OpenTwin 

def __main__(this):
    portData = OpenTwin.GetPortData("DataInput")
    portMetadata = OpenTwin.GetPortMetaData("DataInput")

    offset = OpenTwin.GetPropertyValue(this,"Offset")
    dataWithOffset = []
    for data in portData:
        dataWithOffset.append(data["Magnitude"] + offset)
    
    OpenTwin.SetPortData("DataOutput",dataWithOffset)
    OpenTwin.SetPortMetaData("DataOutput",dataWithOffset)
    
    
