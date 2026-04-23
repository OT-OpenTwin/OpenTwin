#@ Port : {"type": "out", "name": "Dummy", "label" : "Dummy"}
import OpenTwin

def __main__(x):
    propertyGroups = OpenTwin.GetPropertyGroups("Plots/Plot/Curve")
    print(propertyGroups)

