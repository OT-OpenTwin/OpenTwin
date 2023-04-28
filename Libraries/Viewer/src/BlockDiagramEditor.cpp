#include "stdafx.h"
#include "BlockDiagramEditor.h"

BlockDiagramEditorScene::BlockDiagramEditorScene()
{
	setBackgroundBrush(QPixmap(QString(qgetenv("OPENTWIN_DEV_ROOT") + "/Deployment/icons/Images/GridPatternCross.png")));
}
