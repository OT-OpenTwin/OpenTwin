'#Language "WWB-COM"

Option Explicit

Dim count As Long
Dim baseFolder As String


Sub ExportSolidChildItem(parent$)

	Dim item As String
	item = Resulttree.GetFirstChildName(parent$)

	If item = "" Then
		Dim itemName As String
		itemName = Right(parent, Len(parent)-11)
		Dim index As Long
		index = InStrRev(itemName, "\")
		Dim solidName As String
		Dim componentName As String
		solidName = Right(itemName, Len(itemName)-index)
		componentName = Left(itemName, index-1)

		componentName = Replace(componentName, "\", "/")

		With STL
			.Reset
			.FileName (baseFolder + "/stl" + CStr(count) + ".stl")
		    .Name (solidName)
	   		.Component (componentName)
    		.ExportFromActiveCoordinateSystem (False)
    		.Write
		End With

		Print #1, itemName
		Print #1, Solid.GetMaterialNameForShape(componentName + ":" + solidName)

		count = count + 1
	End If

	While item <> ""
		ExportSolidChildItem(item)
		item = Resulttree.GetNextItemName(item)
	Wend

End Sub

Sub ExportMaterials()

	Dim index As Long
	For index = 0 To Material.GetNumberOFMaterials()-1

		Dim item As String
		item = Material.GetNameOfMaterialFromIndex(index)

		Dim materialType As String
		materialType = Material.GetTypeOfMaterial(item)

		Dim r As Double
		Dim g As Double
		Dim b As Double
		Material.GetColour(item, r, g, b)

		Print #1, item
		Print #1, r; " "; g; " "; b
		Print #1, materialType

		If materialType = "Normal" Then
			Dim x As Double
			Dim y As Double
			Dim z As Double
			Material.GetEpsilon(item, x, y, z)
			Print #1, x; " "; y; " "; z
			Material.GetMu(item, x, y, z)
			Print #1, x; " "; y; " "; z
			Material.GetSigma(item, x, y, z)
			Print #1, x; " "; y; " "; z
		End If
	Next
End Sub


Sub ExportUnits()
     Print #1, Units.GetUnit("Length")
     Print #1, Units.GetUnit("Temperature")
     Print #1, Units.GetUnit("Voltage")
     Print #1, Units.GetUnit("Current")
     Print #1, Units.GetUnit("Resistance")
     Print #1, Units.GetUnit("Conductance")
     Print #1, Units.GetUnit("Capacitance")
     Print #1, Units.GetUnit("Inductance")
     Print #1, Units.GetUnit("Frequency")
     Print #1, Units.GetUnit("Time")
End Sub




Sub Main

	SetLock(True)

	baseFolder = GetProjectPath("Temp") + "/Upload"
	On Error Resume Next
	MkDir baseFolder
	count = 0

	ScreenUpdating(False)
	Dim currentItem As String
	currentItem = GetSelectedTreeItem()

	Open baseFolder + "/shape.info" For Output As #1
	ExportSolidChildItem("Components")
	Close #1

	SelectTreeItem(currentItem)
	ScreenUpdating(True)

	Open baseFolder + "/material.info" For Output As #1
	ExportMaterials()
	Close #1

	Open baseFolder + "/units.info" For Output As #1
	ExportUnits()
	Close #1
End Sub
