//! @file BlockPickerWidget.cpp
//! @author Alexander Kuester (alexk95)
//! @date May 2023
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTBlockEditor/BlockPickerWidget.h"
#include "OTBlockEditor/Block.h"
#include "OTBlockEditor/BlockFactory.h"
#include "OTBlockEditor/BlockNetwork.h"
#include "OTBlockEditor/BlockNetworkEditor.h"
#include "OTBlockEditorAPI/BlockConfiguration.h"
#include "OTBlockEditorAPI/BlockConfigurationFactory.h"
#include "OTBlockEditorAPI/BlockCategoryConfiguration.h"
#include "OTWidgets/TreeWidgetFilter.h"
#include "OTWidgets/TreeWidget.h"
#include "OpenTwinCore/otAssert.h"

// Qt header
#include <QtWidgets/qlayout.h>
#include <QtWidgets/qsplitter.h>
#include <QtWidgets/qgraphicsview.h>
#include <QtWidgets/qgraphicsscene.h>
