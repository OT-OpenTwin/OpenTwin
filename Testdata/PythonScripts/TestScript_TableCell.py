#@ Port : {"type": "out", "name": "DisplayValue", "label" : "DisplayValue"}
import OpenTwin

def __main__(this):
    cellValue = OpenTwin.GetTableCellValue("Text Files/Prüfmatrix_S8_Ausschnitt.csv",4,0)
    OpenTwin.SetPortData("DisplayValue",cellValue)
