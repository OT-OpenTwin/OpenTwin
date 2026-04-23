#@ Port : {"type": "out", "name": "Dummy", "label" : "Dummy"}

import OpenTwin
import os

def __main__(this):
    recursiveSearch = True
    folderContent = OpenTwin.GetFolderItemNames("Dataset", recursiveSearch)
    print(folderContent)
