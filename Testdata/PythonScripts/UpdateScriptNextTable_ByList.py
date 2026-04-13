#Requires that the BAT Nr selection is updated first.
import OpenTwin
from pathlib import Path

def __main__(this):
        
    listOfFileNames =  [
        "C1 Hor_Rad400_H100_Deg000_00000",
"C1 Hor_Rad400_H100_Deg000_00001",
"C1 Hor_Rad400_H100_Deg000_00002",
"C1 Hor_Rad400_H100_Deg030_00000",
"C1 Hor_Rad400_H100_Deg030_00001",
"C1 Hor_Rad400_H100_Deg030_00002",
"C1 Hor_Rad400_H100_Deg060_00000",
"C1 Hor_Rad400_H100_Deg060_00001",
"C1 Hor_Rad400_H100_Deg060_00002",
"C1 Hor_Rad400_H100_Deg090_00000",
"C1 Hor_Rad400_H100_Deg090_00001",
"C1 Hor_Rad400_H100_Deg090_00002",
"C1 Hor_Rad400_H100_Deg120_00000",
"C1 Hor_Rad400_H100_Deg120_00001",
"C1 Hor_Rad400_H100_Deg120_00002",
"C1 Hor_Rad400_H100_Deg150_00000",
"C1 Hor_Rad400_H100_Deg150_00001",
"C1 Hor_Rad400_H100_Deg150_00002"]
    currentTableName = OpenTwin.GetPropertyValue(this,"Table name")
    currentTableName = Path(currentTableName).stem 
    idx = listOfFileNames.index(currentTableName)
    if idx + 1 < len(listOfFileNames):    
        next_entry = listOfFileNames[idx + 1]
    else:
        raise Exception("Reached end of table name list. ")
    
    newTableName = "Text Files/" + next_entry + ".csv"
    setSuccess = OpenTwin.SetPropertyValue(this,"Table name",newTableName)
    if not setSuccess:    
        raise Exception('Failed to update property of ', this)