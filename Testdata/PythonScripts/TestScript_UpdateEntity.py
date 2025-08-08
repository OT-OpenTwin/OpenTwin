#@ Port : {"type": "out", "name": "Dummy", "label" : "Dummy"}
import OpenTwin

def __main__(x):
    factor = OpenTwin.GetPropertyValue("Units","Time")
    newValue = "ms"
    setSuccess = OpenTwin.SetPropertyValue("Units","Time",newValue)
    flushSuccess = OpenTwin.FlushEntity("Units")
