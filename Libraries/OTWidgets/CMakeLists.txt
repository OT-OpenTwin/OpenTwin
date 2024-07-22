cmake_minimum_required(VERSION 3.15)

project(OTWidgets LANGUAGES CXX)

# Set the Qt installation directory
set(CMAKE_PREFIX_PATH "${CMAKE_SOURCE_DIR}/QT/5.15.2/msvc2019_64")

# Find the required Qt modules
find_package(Qt5 COMPONENTS Core Widgets Gui REQUIRED)

add_library(OTWidgets SHARED
    src/ApplicationPropertiesManager.cpp
    src/BasicWidgetView.cpp
    src/CheckBox.cpp
    src/ColorPickButton.cpp
    src/ColorPreviewBox.cpp
    src/ColorStyle.cpp
    src/ColorStyleValue.cpp
    src/ComboBox.cpp
    src/ComboButton.cpp
    src/CustomGraphicsItem.cpp
    src/Dialog.cpp
    src/dllmain.cpp
    src/DockWatcherAction.cpp
    src/DoubleSpinBox.cpp
    src/EventNotifier.cpp
    src/FilePathEdit.cpp
    src/GlobalColorStyle.cpp
    src/GraphicsArcItem.cpp
    src/GraphicsBoxLayoutItem.cpp
    src/GraphicsConnectionItem.cpp
    src/GraphicsConnectionPreviewItem.cpp
    src/GraphicsEllipseItem.cpp
    src/GraphicsItemFactory.cpp
    src/GraphicsGridLayoutItem.cpp
    src/GraphicsGroupItem.cpp
    src/GraphicsHBoxLayoutItem.cpp
    src/GraphicsItem.cpp
    src/GraphicsItemLoader.cpp
    src/GraphicsItemPreviewDrag.cpp
    src/GraphicsItemPreview.cpp
    src\GraphicsInvisibleItem.cpp
    src/GraphicsLayoutItem.cpp
    src/GraphicsLayoutItemWrapper.cpp
    src/GraphicsLineItem.cpp
    src/GraphicsPicker.cpp
    src/GraphicsPickerView.cpp
    src/GraphicsPixmapItem.cpp
    src/GraphicsPolygonItem.cpp
    src/GraphicsRectangularItem.cpp
    src/GraphicsScene.cpp
    src/GraphicsShapeItem.cpp
    src/GraphicsStackItem.cpp
    src/GraphicsTextItem.cpp
    src/GraphicsTriangleItem.cpp
    src/GraphicsVBoxLayoutItem.cpp
    src/GraphicsView.cpp
    src/GraphicsViewView.cpp
    src/IconManager.cpp
    src/ImagePreview.cpp
    src/Label.cpp
    src/LineEdit.cpp
    src/MessageDialog.cpp
    src/OnePropertyDialog.cpp
    src/OverlayWidgetBase.cpp
    src/Painter2DEditButton.cpp
    src/Painter2DEditDialog.cpp
    src/Painter2DPreview.cpp
    src/PlainTextEditView.cpp
    src/Positioning.cpp
    src/QtFactory.cpp
    src/PlainTextEdit.cpp
    src/PropertyDialog.cpp
    src/PropertyGridItemDelegate.cpp
    src/PropertyGridView.cpp
    src/PropertyInputBool.cpp
    src/PropertyInputColor.cpp
    src/PropertyInputDirectory.cpp
    src/PropertyInputDouble.cpp
    src/PropertyInputFactory.cpp
    src/PropertyGrid.cpp
    src/PropertyGridItem.cpp
    src/PropertyGridGroup.cpp
    src/PropertyInput.cpp
    src/PropertyInputFilePath.cpp
    src/PropertyInputInt.cpp
    src/PropertyInputPainter2D.cpp
    src/PropertyInputString.cpp
    src/PropertyInputStringList.cpp
    src/PushButton.cpp
    src/QWidgetInterface.cpp
    src/SpinBox.cpp
    src/Splitter.cpp
    src/TabWidget.cpp
    src/TextEditor.cpp
    src/TextEditorSearchPopup.cpp
    src/TextEditorView.cpp
    src/ToolTipHandler.cpp
    src/TreeWidget.cpp
    src/TreeWidgetFilter.cpp
    src/TreeWidgetItem.cpp
    src/TreeWidgetItemInfo.cpp
    src/WidgetView.cpp
    src/WidgetViewFactory.cpp
    src/WidgetViewManager.cpp
)

target_include_directories(OTWidgets PRIVATE
    ${Qt5Core_INCLUDE_DIRS}
    ${Qt5Widgets_INCLUDE_DIRS}
    ${Qt5Gui_INCLUDE_DIRS}
)

target_link_libraries(OTWidgets PRIVATE
    Qt5::Core
    Qt5::Widgets
    Qt5::Gui
)
