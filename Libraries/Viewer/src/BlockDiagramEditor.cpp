#include "stdafx.h"
#include "BlockDiagramEditor.h"

BlockDiagramEditorScene::BlockDiagramEditorScene()
{
	setBackgroundBrush(QPixmap(QString(qgetenv("SIM_PLAT_ROOT") + "/Deployment/icons/Images/GridPatternCross.png")));
}
