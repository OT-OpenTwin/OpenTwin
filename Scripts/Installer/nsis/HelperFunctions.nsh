;Helper functions
!ifndef HELPER_FUNCTIONS
!define HELPER_FUNCTIONS

var currentSectionFlags
Function SetSectionFlag
	SectionGetFlags $9 $currentSectionFlags
	IntOp $currentSectionFlags $currentSectionFlags | $8
	SectionSetFlags $9 $currentSectionFlags
FunctionEnd


Function UnsetSectionFlag
	SectionGetFlags $9 $currentSectionFlags
	IntOp $currentSectionFlags $currentSectionFlags & ~$8
	SectionSetFlags $9 $currentSectionFlags
FunctionEnd

!endif ; MY_HEADER_INCLUDED