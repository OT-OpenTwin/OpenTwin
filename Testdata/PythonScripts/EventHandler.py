#@ Event : {"name": "Table Filter Changed", "function": "handle1"}
# Possible events: "Item Deleted", "Task Started", "Task Finished", "Table Filter Changed", "Property Updated"

import OpenTwin 
import json

def handle1(this, **kwargs):
	print("Handle table filter changed event")

def __main__(self, **kwargs):
    # Works with no kwargs — kwargs is just an empty dict {}
    # Works with kwargs  — kwargs["measurements"], kwargs["scale"], etc.
    print("Main")