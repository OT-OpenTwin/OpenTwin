#@ Port : {"type": "out", "name": "DisplayValue", "label" : "DisplayValue"}
import numpy as np 
import OpenTwin
import json

def __main__(this):  
    # creating a 2X2 Numpy matrix 
    n_array = np.array([[6,-1.7], [30, 44]])
      
    # Displaying the Matrix 
    print("Numpy Matrix is:") 
    print(n_array) 
      
    # calculating the determinant of matrix 
    det = np.linalg.det(n_array) 
      
    print("\nDeterminant of given 2X2 matrix:") 
    print(int(det))
    data = {"Determinant": int(det)}
    OpenTwin.SetPortData("DisplayValue",json.dumps(data))