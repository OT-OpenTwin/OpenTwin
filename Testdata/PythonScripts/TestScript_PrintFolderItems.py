#@ Port : {"type": "out", "name": "Dummy", "label" : "Dummy"}

import OpenTwin
import os

def __main__(this):
    folderContent = OpenTwin.GetFolderItemNames(Dataset)
    print(folderContent)
