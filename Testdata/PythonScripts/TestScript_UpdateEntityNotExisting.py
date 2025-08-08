import OpenTwin

def __main__(x):
    factor = OpenTwin.GetPropertyValue("Materials/material1","Not a property")
    newValue = x * factor
    setSuccess = OpenTwin.SetPropertyValue("Materials/material1","Permittivity (relative)",newValue)
    flushSuccess = OpenTwin.FlushEntity("Materials/material1")
