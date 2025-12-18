"""
Dummy runtime module for OpenTWin.

This module exists only to satisfy the Python interpreter at runtime.
The real OpenTWin module is provided by the embedded OpenTwin environment.
"""

import sys

if not hasattr(sys, "_opentwin_dummy_loaded"):
    print(
        "[OpenTwin] Dummy runtime module loaded. "
        "Real OpenTWin module is provided by the embedded interpreter."
    )
    sys._opentwin_dummy_loaded = True

# --- Dummy functions matching the .pyi ---
def GetPropertyValue(*args, **kwargs):
    print("[OpenTwin] Called GetPropertyValue with", args, kwargs)
    return None

def GetTableCellValue(*args, **kwargs):
    print("[OpenTwin] Called GetTableCellValue with", args, kwargs)
    return None

def SetPropertyValue(*args, **kwargs):
    print("[OpenTwin] Called SetPropertyValue with", args, kwargs)
    return True

def Flush():
    print("[OpenTwin] Called Flush()")
    return True

def FlushEntity(*args, **kwargs):
    print("[OpenTwin] Called FlushEntity with", args, kwargs)
    return True

def GetPythonScript(*args, **kwargs):
    print("[OpenTwin] Called GetPythonScript with", args, kwargs)
    return None

def GetPortData(*args, **kwargs):
    print("[OpenTwin] Called GetPortData with", args, kwargs)
    return None

def GetPortMetaData(*args, **kwargs):
    print("[OpenTwin] Called GetPortMetaData with", args, kwargs)
    return None

def SetPortData(*args, **kwargs):
    print("[OpenTwin] Called SetPortData with", args, kwargs)
    return True

def SetPortMetaData(*args, **kwargs):
    print("[OpenTwin] Called SetPortMetaData with", args, kwargs)
    return True