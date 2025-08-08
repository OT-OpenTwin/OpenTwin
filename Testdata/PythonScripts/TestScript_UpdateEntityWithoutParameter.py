#@ Port : {"type": "out", "name": "Dummy", "label" : "Dummy"}
import OpenTwin

def __main__(this):
    factor = OpenTwin.GetPropertyValue("Materials/material1","Permittivity (relative)")
    newValue = 2 * factor
    setSuccess = OpenTwin.SetPropertyValue("Materials/material1","Permittivity (relative)",newValue)
    flushSuccess = OpenTwin.FlushEntity("Materials/material1")
    OpenTwin.SetPortData("Dummy",flushSuccess)
