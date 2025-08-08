#@ Port : {"type": "out", "name": "Dummy", "label" : "Dummy"}
import OpenTwin

def __main__(this):
    factor = OpenTwin.GetPropertyValue("Materials/material1","NotExisting")
    newValue = 2 * factor
    setSuccess = OpenTwin.SetPropertyValue("Materials/material1","NotExisting",newValue)
    flushSuccess = OpenTwin.FlushEntity("Materials/material1")
