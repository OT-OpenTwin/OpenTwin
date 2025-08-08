#Requires that the BAT Nr selection is updated first.
import OpenTwin

def find_nth(haystack, needle, n):
    start = haystack.find(needle)
    while start >= 0 and n > 1:
        start = haystack.find(needle, start+len(needle))
        n -= 1
    return start

def __main__(this):
    prefixEnd = find_nth(this,"/",3)
    prefix = this[0:prefixEnd]
    selectionReference = prefix + "/Prüfnr." #This is the name of a selection entity in the navigation tree. It holds a value that is used in the name of the corresponding table
    currentRow = OpenTwin.GetPropertyValue(selectionReference,"Top row")
    
    originalTableName = OpenTwin.GetPropertyValue(this,"Table name")
    cellValue = OpenTwin.GetTableCellValue("Text Files/Prüfmatrix_S8_Ausschnitt.csv",currentRow,0) # The Prüfnr. entry is in the first column, (internally referenced as zero-base index)
    newTableName = "Text Files/S8_" + cellValue + "_FA.csv"
    setSuccess = OpenTwin.SetPropertyValue(this,"Table name",newTableName)
    if not setSuccess:    
        raise Exception('Failed to update property of ', this)

