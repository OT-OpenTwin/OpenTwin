#@ Port : {"type": "in", "name": "S_Parameter", "label" : "S Parameter Matrix"}
#@ Port : {"type": "out", "name": "ZParameter", "label" : " Nodal Z-Parameter"}
#@ Property : {"label" : "Reference Impedance","type" : "double", "default" : 50.0}
import OpenTwin 
import numpy as np


def BuildIdentityMatrix(dimension):
	return np.identity(dimension)

def __main__(this):
    Sse = OpenTwin.GetPortData("S_Parameter")[0]
    matrixShape = Sse.shape
    I = BuildIdentityMatrix(matrixShape[0])
    Z0 = 50
    Zn = Z0 * np.linalg.inv(I - Sse)*  (I+Sse)
    OpenTwin.SetPortData("ZParameter",Zn)
 #   Z0 = OpenTwin.GetPropertyValue(this,"ReferenceImpedance")
 
	
       
    
    
    
#Port : {"type": "in", "name": "ReferenceImpedance", "label" : "Reference Impedance"}
    
