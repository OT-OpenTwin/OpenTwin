# Possible events: "Item Deleted", "Task Started", "Task Finished", "Table Filter Changed", "Property Updated"
#@ Event : {"name": "Table Filter Changed", "function": "handle1"}

import OpenTwin 
import json

def handle1(this):
	print("Handle table filter changed event")

def __main__(this):
    print("This should not be executed.")
    
    
