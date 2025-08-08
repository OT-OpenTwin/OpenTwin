#@ Port : {"type": "out", "name": "DisplayValue", "label" : "DisplayValue"}
import OpenTwin

def __main__(this):
    testScript = OpenTwin.GetPythonScript("Scripts/TestScript_TableCell.py")
    testScript(this)


