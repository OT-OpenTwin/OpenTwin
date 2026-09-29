# License:
# Copyright 2026 by OpenTwin
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#     http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.

"""What CreateDeployment and UpdateDeploymentLibrariesOnly copy.

Edit this file to change the contents of a deployment. Each entry mirrors one
line of the matching batch script.

    plan.copy(source, target)          COPY, wildcards allowed
    plan.tree(source, target)          XCOPY /S, add empty=True for /E
    plan.mkdir(target)                 MKDIR
    plan.rmtree(target)                RMDIR /S /Q
    plan.remove(target)                DEL, wildcards allowed
    plan.remove_glob(folder, pattern)  recursive DEL
    plan.write(target, lines)          ECHO > file
    plan.action(name)                  a step ot_dev runs (see ACTIONS)
"""

QT_CONF = ("[Paths] ", r"Plugins = .\\plugins ")


MAKENSIS = r"C:\Program Files (x86)\NSIS\makensis.exe"

NSI = "Install-OpenTwin.nsi"

INSTALLER_DIR = r"%OPENTWIN_DEV_ROOT%\Scripts\BuildAndTest"


def update_libraries(plan) -> None:
    # Shutdown
    plan.action("shutdown")

    # Previous build
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\DebugService.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\OTSystem.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\OTCore.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\OTCommunication.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\OTServiceFoundation.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\OTModelEntities.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\OTCADEntities.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\OTBlockEntities.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\Model.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\OTModelAPI.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\OTViewer.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\OTDataStorage.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\uiCore.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\OTRubberband.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\OTRubberbandOSG.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\GlobalSessionService.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\LocalSessionService.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\GlobalDirectoryService.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\LocalDirectoryService.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\uiFrontend.exe")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\RelayService.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\LoggerService.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\AuthorisationService.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\PHREECService.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\ModelingService.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\VisualizationService.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\FITTDService.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\PythonExecutionService.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\PythonExecution.exe")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\ImportParameterizedDataService.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\DataProcessingService.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\CartesianMeshService.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\TetMeshService.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\GetDPService.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\ElmerFEMService.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\StudioSuiteService.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\LTSpiceService.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\PyritService.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\OTGui.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\OTGuiAPI.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\OTWidgets.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\OTFrontendConnectorAPI.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\OTFMC.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\OTStudioSuiteConnector.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\OTLTSpiceConnector.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\open_twin.exe")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\CircuitSimulatorService.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\CircuitExecution.exe")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\OToolkit.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\OTResultDataAccess.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\OTSystemInformationTool.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\LibraryManagementService.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\HierarchicalProjectService.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\FileManagementProjectService.dll")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\OpenEMSService.dll")

    # Data folders
    plan.rmtree(r"%OPENTWIN_DEPLOYMENT_DIR%\LibraryData")
    plan.remove(r"%OPENTWIN_DEPLOYMENT_DIR%\ColorStyles\*.otcsf")
    plan.rmtree(r"%OPENTWIN_DEPLOYMENT_DIR%\GraphicsItems")
    plan.mkdir(r"%OPENTWIN_DEPLOYMENT_DIR%\Apache")
    plan.rmtree(r"%OPENTWIN_DEPLOYMENT_DIR%\Apache\htdocs")
    plan.mkdir(r"%OPENTWIN_DEPLOYMENT_DIR%\Apache\htdocs")

    # Libraries
    plan.copy(r"%OT_CORE_ROOT%\%OT_CDLLR%\OTCore.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_SYSTEM_ROOT%\%OT_CDLLR%\OTSystem.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_COMMUNICATION_ROOT%\%OT_CDLLR%\OTCommunication.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_FOUNDATION_ROOT%\%OT_CDLLR%\OTServiceFoundation.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_MODELENTITIES_ROOT%\%OT_CDLLR%\OTModelEntities.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_CADMODELENTITIES_ROOT%\%OT_CDLLR%\OTCADEntities.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_VIEWER_ROOT%\%OT_CDLLR%\OTViewer.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_DATASTORAGE_ROOT%\%OT_CDLLR%\OTDataStorage.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_UICORE_ROOT%\%OT_CDLLR%\uiCore.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_RUBBERBANDAPI_ROOT%\%OT_CDLLR%\OTRubberband.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_RUBBERBAND_OSG_ROOT%\%OT_CDLLR%\OTRubberbandOSG.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_GUI_ROOT%\%OT_CDLLR%\OTGui.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_GUIAPI_ROOT%\%OT_CDLLR%\OTGuiAPI.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_MODELAPI_ROOT%\%OT_CDLLR%\OTModelAPI.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_WIDGETS_ROOT%\%OT_CDLLR%\OTWidgets.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_FRONTEND_CONNECTOR_API_ROOT%\%OT_CDLLR%\OTFrontendConnectorAPI.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_FILE_MANAGER_CONNECTOR_ROOT%\%OT_CDLLR%\OTFMC.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_STUDIO_SUITE_CONNECTOR_ROOT%\%OT_CDLLR%\OTStudioSuiteConnector.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_LTSPICE_CONNECTOR_ROOT%\%OT_CDLLR%\OTLTSpiceConnector.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_BLOCKENTITIES_ROOT%\%OT_CDLLR%\OTBlockEntities.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_RESULT_DATA_ACCESS_ROOT%\%OT_CDLLR%\OTResultDataAccess.dll", "%OPENTWIN_DEPLOYMENT_DIR%")

    # Services
    plan.copy(r"%OT_MODEL_SERVICE_ROOT%\%OT_CDLLR%\Model.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_GLOBAL_SESSION_SERVICE_ROOT%\%OT_CDLLR%\GlobalSessionService.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_LOCAL_SESSION_SERVICE_ROOT%\%OT_CDLLR%\LocalSessionService.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_GLOBAL_DIRECTORY_SERVICE_ROOT%\%OT_CDLLR%\GlobalDirectoryService.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_LOCAL_DIRECTORY_SERVICE_ROOT%\%OT_CDLLR%\LocalDirectoryService.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_UI_SERVICE_ROOT%\%OT_CDLLR%\uiFrontend.exe", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_RELAY_SERVICE_ROOT%\%OT_CDLLR%\RelayService.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_LOGGER_SERVICE_ROOT%\%OT_CDLLR%\LoggerService.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_AUTHORISATION_SERVICE_ROOT%\%OT_CDLLR%\AuthorisationService.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_PHREEC_SERVICE_ROOT%\%OT_CDLLR%\PHREECService.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_MODELING_SERVICE_ROOT%\%OT_CDLLR%\ModelingService.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_VISUALIZATION_SERVICE_ROOT%\%OT_CDLLR%\VisualizationService.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_FITTD_SERVICE_ROOT%\%OT_CDLLR%\FITTDService.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_IMPORT_PARAMETERIZED_DATA_SERVICE_ROOT%\%OT_CDLLR%\ImportParameterizedDataService.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_CARTESIAN_MESH_SERVICE_ROOT%\%OT_CDLLR%\CartesianMeshService.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_TET_MESH_SERVICE_ROOT%\%OT_CDLLR%\TetMeshService.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_GETDP_SERVICE_ROOT%\%OT_CDLLR%\GetDPService.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_ELMERFEM_SERVICE_ROOT%\%OT_CDLLR%\ElmerFEMService.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_STUDIOSUITE_SERVICE_ROOT%\%OT_CDLLR%\StudioSuiteService.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_LTSPICE_SERVICE_ROOT%\%OT_CDLLR%\LTSpiceService.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_PYRIT_SERVICE_ROOT%\%OT_CDLLR%\PyritService.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_PYTHON_EXECUTION_SERVICE_ROOT%\%OT_CDLLR%\PythonExecutionService.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_PYTHON_EXECUTION_ROOT%\%OT_CDLLR%\PythonExecution.exe", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_DEBUGSERVICE_ROOT%\%OT_CDLLR%\DebugService.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_DATA_PROCESSING_SERVICE_ROOT%\%OT_CDLLR%\DataProcessingService.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_CIRCUIT_SIMULATOR_SERVICE_ROOT%\%OT_CDLLR%\CircuitSimulatorService.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_CIRCUIT_EXECUTION_ROOT%\%OT_CDLLR%\CircuitExecution.exe", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_LIBRARY_MANAGEMENT_SERVICE_ROOT%\%OT_CDLLR%\LibraryManagementService.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_HIERARCHICAL_PROJECT_SERVICE_ROOT%\%OT_CDLLR%\HierarchicalProjectService.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_FILEMANAGEMENT_PROJECT_SERVICE_ROOT%\%OT_CDLLR%\FileManagementProjectService.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_OPENEMS_SERVICE_ROOT%\%OT_CDLLR%\OpenEMSService.dll", "%OPENTWIN_DEPLOYMENT_DIR%")

    # Framework and tools
    plan.copy(r"%OPENTWIN_DEV_ROOT%\Framework\OpenTwin\target\release\open_twin.exe", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OPENTWIN_DEV_ROOT%\Tools\OToolkitAPI\%OT_CDLLR%\OToolkitAPI.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OPENTWIN_DEV_ROOT%\Tools\OToolkit\%OT_CDLLR%\OToolkit.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OPENTWIN_DEV_ROOT%\Tools\OTSystemInformationTool\%OT_CDLLR%\OTSystemInformationTool.exe", "%OPENTWIN_DEPLOYMENT_DIR%")

    # Data
    plan.tree(r"%OPENTWIN_DEV_ROOT%\LibraryData", r"%OPENTWIN_DEPLOYMENT_DIR%\LibraryData", empty=True)
    plan.copy(r"%OPENTWIN_DEV_ROOT%\Assets\ColorStyles\*.otcsf", r"%OPENTWIN_DEPLOYMENT_DIR%\ColorStyles")
    plan.mkdir(r"%OPENTWIN_DEPLOYMENT_DIR%\GraphicsItems")
    plan.tree(r"%OPENTWIN_DEV_ROOT%\Assets\GraphicsItems\*.ot.json", r"%OPENTWIN_DEPLOYMENT_DIR%\GraphicsItems")

    # Admin panel
    plan.tree(r"%OPENTWIN_DEV_ROOT%\Tools\AdminPanel\build\*.*", r"%OPENTWIN_DEPLOYMENT_DIR%\Apache\htdocs", empty=True)
    plan.copy(r"%OPENTWIN_DEV_ROOT%\Tools\AdminPanel\Apache_config\.htaccess", r"%OPENTWIN_DEPLOYMENT_DIR%\Apache\htdocs")
    plan.copy(r"%OPENTWIN_DEV_ROOT%\Tools\AdminPanel\Apache_config\httpd.conf", r"%OPENTWIN_DEPLOYMENT_DIR%\Apache\conf")
    plan.copy(r"%OPENTWIN_DEV_ROOT%\Tools\AdminPanel\Apache_config\httpd-ahssl.conf", r"%OPENTWIN_DEPLOYMENT_DIR%\Apache\conf\extra")

    # Build information
    plan.action("build information")


def create_deployment(plan) -> None:
    # Shutdown
    plan.action("shutdown")

    # Clean up
    plan.rmtree("%OT_DEPLOYMENT_DIR%")
    plan.rmtree(r"%OPENTWIN_DEV_ROOT%\Deployment_Documentation")
    plan.mkdir("%OT_DEPLOYMENT_DIR%")
    plan.mkdir(r"%OT_DEPLOYMENT_DIR%\Certificates")

    # Qwt
    plan.copy(r"%QWT_LIB_DLLR%\qwt.dll", "%OT_DEPLOYMENT_DIR%")

    # QtTabToolbar
    plan.copy(r"%QT_TT_DLLR%\TabToolbar.dll", "%OT_DEPLOYMENT_DIR%")

    # Qt AdvancedDockingSystem
    plan.copy(r"%QT_ADS_ROOT%\lib\qtadvanceddocking-qt6.dll", "%OT_DEPLOYMENT_DIR%")

    # OpenSceneGraph
    plan.copy(r"%OSG_DLLR%\*.dll", "%OT_DEPLOYMENT_DIR%")
    plan.mkdir(r"%OT_DEPLOYMENT_DIR%\osgPlugins-3.6.3")
    plan.tree(r"%OSG_DLLR%\osgPlugins-3.6.3", r"%OT_DEPLOYMENT_DIR%\osgPlugins-3.6.3")

    # Fonts
    plan.mkdir(r"%OT_DEPLOYMENT_DIR%\fonts")
    plan.tree(r"%OPENTWIN_DEV_ROOT%\Assets\Fonts", r"%OT_DEPLOYMENT_DIR%\fonts")

    # OpenGL Software Rendering
    plan.copy(r"%OPENTWIN_THIRDPARTY_ROOT%\MesaOpenGL\*.*", "%OT_DEPLOYMENT_DIR%")

    # Qt
    plan.copy(r"%QT_DLLR%\Qt63DAnimation.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt63DCore.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt63DExtras.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt63DInput.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt63DLogic.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt63DQuick.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt63DQuickAnimation.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt63DQuickExtras.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt63DQuickInput.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt63DQuickRender.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt63DQuickScene2D.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt63DRender.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt6Bluetooth.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt6Charts.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt6Concurrent.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt6Core.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt6DataVisualization.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt6DBus.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt6Gui.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt6Help.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt6Location.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt6Multimedia.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt6MultimediaQuick.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt6MultimediaWidgets.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt6Network.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt6NetworkAuth.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt6Nfc.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt6OpenGL.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt6OpenGLWidgets.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt6Positioning.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt6PositioningQuick.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt6PrintSupport.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt6Qml.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt6Quick.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt6QuickControls2.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt6QuickParticles.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt6QuickTemplates2.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt6QuickTest.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt6QuickWidgets.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt6RemoteObjects.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt6Scxml.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt6Sensors.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt6SerialBus.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt6SerialPort.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt6Sql.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt6Svg.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt6SvgWidgets.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt6Test.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt6TextToSpeech.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt6WebChannel.dll", "%OT_DEPLOYMENT_DIR%")
    # plan.copy(r"%QT_DLLR%\Qt6WebEngineWidgets.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt6WebSockets.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt6WebView.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt6Widgets.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%QT_DLLR%\Qt6Xml.dll", "%OT_DEPLOYMENT_DIR%")

    # Qt plugins
    plan.mkdir(r"%OT_DEPLOYMENT_DIR%\plugins")
    plan.mkdir(r"%OT_DEPLOYMENT_DIR%\plugins\imageformats")
    plan.mkdir(r"%OT_DEPLOYMENT_DIR%\plugins\platforms")
    plan.mkdir(r"%OT_DEPLOYMENT_DIR%\plugins\renderers")
    plan.mkdir(r"%OT_DEPLOYMENT_DIR%\plugins\tls")
    plan.tree(r"%QDIR%\plugins\imageformats", r"%OT_DEPLOYMENT_DIR%\plugins\imageformats")
    plan.tree(r"%QDIR%\plugins\platforms", r"%OT_DEPLOYMENT_DIR%\plugins\platforms")
    plan.tree(r"%QDIR%\plugins\renderers", r"%OT_DEPLOYMENT_DIR%\plugins\renderers")
    plan.tree(r"%QDIR%\plugins\tls", r"%OT_DEPLOYMENT_DIR%\plugins\tls")
    plan.remove_glob(r"%OT_DEPLOYMENT_DIR%\plugins", "*.pdb")

    # OpenSSL websocket
    plan.copy(r"%OPENSSL_WEBSOCKET_DLLR%\*.dll", "%OT_DEPLOYMENT_DIR%")

    # Boost
    plan.copy(r"%BOOST_DLLPATHR%\*.dll", "%OT_DEPLOYMENT_DIR%")

    # CGAL
    plan.copy(r"%GMP_DLLPATHR%\*.dll", "%OT_DEPLOYMENT_DIR%")

    # OpenCascade
    plan.copy(r"%FMP_DLLR%\*.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%FRI_DLLR%\*.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%FRT_DLLR%\*.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%OC_DLLR%\*.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%TBB_DLLR%\tbb12.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%TBB_DLLR%\tbbmalloc.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%OVR_DLLR%\*.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%JEM_DLLR%\*.dll", "%OT_DEPLOYMENT_DIR%")

    # VTK
    plan.copy(r"%VTK_DLLR%\*.dll", "%OT_DEPLOYMENT_DIR%")

    # Icons
    plan.mkdir(r"%OT_DEPLOYMENT_DIR%\icons")
    plan.tree(r"%OPENTWIN_DEV_ROOT%\Assets\Icons", r"%OT_DEPLOYMENT_DIR%\icons")

    # ColorStyles
    plan.mkdir(r"%OT_DEPLOYMENT_DIR%\ColorStyles")
    plan.tree(r"%OPENTWIN_DEV_ROOT%\Assets\ColorStyles", r"%OT_DEPLOYMENT_DIR%\ColorStyles")

    # GMSH
    plan.copy(r"%GMSH_ROOT_BIN%\gmsh.dll", "%OT_DEPLOYMENT_DIR%")

    # Curl
    plan.copy(r"%CURL_DLLR%\libcurl.dll", "%OT_DEPLOYMENT_DIR%")

    # OpenSSL
    plan.copy(r"%OPENSSL_DLL%\libcrypto-1_1-x64.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%OPENSSL_DLL%\libssl-1_1-x64.dll", "%OT_DEPLOYMENT_DIR%")

    # MongoDB
    plan.copy(r"%MONGO_CXX_DLLR%\*.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%MONGO_C_DLLR%\*.dll", "%OT_DEPLOYMENT_DIR%")

    # ZLib
    plan.copy(r"%ZLIB_DLLPATHR%\zlib.dll", "%OT_DEPLOYMENT_DIR%")

    # Embree
    plan.copy(r"%EMBREE_BIN%\embree3.dll", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%EMBREE_BIN%\tbb12.dll", "%OT_DEPLOYMENT_DIR%")

    # GetDP
    plan.copy(r"%GETDP_BIN%\*.*", "%OT_DEPLOYMENT_DIR%")

    # openEMS
    plan.mkdir(r"%OT_DEPLOYMENT_DIR%\openEMSSolver")
    plan.tree(r"%OPENTWIN_THIRDPARTY_ROOT%\openEMS\openEMS_v0.0.36\*.*", r"%OT_DEPLOYMENT_DIR%\openEMSSolver")

    # ElmerFEM
    plan.mkdir(r"%OT_DEPLOYMENT_DIR%\ElmerFEM")
    plan.tree(r"%ELMERFEM_BIN%\*.*", r"%OT_DEPLOYMENT_DIR%\ElmerFEM")

    # Python
    plan.copy(r"%OT_PYTHON_BIN%\Release\python.exe", "%OT_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_PYTHON_BIN%\Release\%OT_PYTHON_BIN_NAME%_release._pth", r"%OT_DEPLOYMENT_DIR%\%OT_PYTHON_BIN_NAME%._pth")
    plan.copy(r"%OT_PYTHON_BIN%\Release\%OT_PYTHON_BIN_NAME%.dll", "%OT_DEPLOYMENT_DIR%")

    # Python environments
    # plan.tree(r"%OT_PYTHON_ROOT%\Environments\PyritEnvironment\*.*", r"%OT_DEPLOYMENT_DIR%\PythonEnvironments\PyritEnvironment")
    plan.tree(r"%OT_PYTHON_ROOT%\Environments\OpenEMSEnvironment\*.*", r"%OT_DEPLOYMENT_DIR%\PythonEnvironments\OpenEMSEnvironment")
    plan.tree(r"%OT_PYTHON_ROOT%\Environments\PythonBuildTools\*.*", r"%OT_DEPLOYMENT_DIR%\PythonEnvironments\PythonBuildTools")
    plan.tree(r"%OT_PYTHON_ROOT%\Environments\CoreEnvironment\Lib\*.*", r"%OT_DEPLOYMENT_DIR%\PythonEnvironments\CoreEnvironment\Lib")
    plan.tree(r"%OT_PYTHON_ROOT%\Environments\CoreEnvironment\DLLs\Release\*.*", r"%OT_DEPLOYMENT_DIR%\PythonEnvironments\CoreEnvironment\DLLs")

    # NGSpice
    plan.copy(r"%NGSPICE_ROOT%\visualc\sharedspice\Release.x64\ngspice.dll", "%OT_DEPLOYMENT_DIR%")

    # Expat
    plan.copy(r"%EXPAT_BIN%\libexpat.dll", "%OT_DEPLOYMENT_DIR%")

    # Apache
    plan.mkdir(r"%OT_DEPLOYMENT_DIR%\Apache")
    plan.tree(r"%APACHE_ROOT%\*.*", r"%OT_DEPLOYMENT_DIR%\Apache")

    # Visual Studio Redistributables
    plan.mkdir(r"%OT_DEPLOYMENT_DIR%\VC_Redist")
    plan.copy(r"%VC_REDIST_ROOT%\*.exe", r"%OT_DEPLOYMENT_DIR%\VC_Redist")

    # Documentation
    plan.mkdir(r"%OPENTWIN_DEV_ROOT%\Deployment_Documentation")
    plan.tree(r"%OT_DOCUMENTATION_ROOT%\_build\html\*.*", r"%OPENTWIN_DEV_ROOT%\Deployment_Documentation")

    # Libraries and services
    update_libraries(plan)

    # Launcher scripts, ShutdownAll and the ot_launcher package
    plan.tree(r"%OPENTWIN_DEV_ROOT%\Scripts\Launcher\*.*", "%OT_DEPLOYMENT_DIR%")

    # Password encryption
    plan.tree(r"%OPENTWIN_DEV_ROOT%\Tools\PasswordEncryption\%OT_CDLLR%\PasswordEncryption.exe", "%OT_DEPLOYMENT_DIR%")

    # Certificates
    plan.tree(r"%OPENTWIN_THIRDPARTY_ROOT%\CertificateCreation\*.*", r"%OT_DEPLOYMENT_DIR%\Certificates")
    plan.tree(r"%OPENTWIN_DEV_ROOT%\Certificates\CreateServerCertificates\*.*", r"%OT_DEPLOYMENT_DIR%\Certificates")
    plan.copy(r"%OPENTWIN_DEV_ROOT%\Certificates\Generated\ca.pem", r"%OT_DEPLOYMENT_DIR%\Certificates")
    plan.copy(r"%OPENTWIN_DEV_ROOT%\Certificates\Generated\server.pem", r"%OT_DEPLOYMENT_DIR%\Certificates")
    plan.copy(r"%OPENTWIN_DEV_ROOT%\Certificates\Generated\server-key.pem", r"%OT_DEPLOYMENT_DIR%\Certificates")
    plan.copy(r"%OPENTWIN_DEV_ROOT%\Certificates\Generated\ca-key.pem", r"%OT_DEPLOYMENT_DIR%\Certificates")
    plan.copy(r"%OPENTWIN_DEV_ROOT%\Certificates\Generated\certificateKeyFile.pem", r"%OT_DEPLOYMENT_DIR%\Certificates")

    # Qt configuration
    plan.write(r"%OT_DEPLOYMENT_DIR%\qt.conf", QT_CONF)


def create_frontend_installer(plan) -> None:
    # Clean up
    plan.rmtree("%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.mkdir("%OPENTWIN_FRONTEND_DEPLOYMENT%")

    # Installer folder
    plan.rmtree(r"%OPENTWIN_DEV_ROOT%\Deployment\FrontendInstaller")
    plan.mkdir(r"%OPENTWIN_DEV_ROOT%\Deployment\FrontendInstaller")

    # Visual C++ runtime
    plan.copy(r"%SYSTEM_32%\downlevel\api-ms-win-crt-runtime-l1-1-0.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%SYSTEM_32%\msvcp140.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%SYSTEM_32%\msvcp140_1.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%SYSTEM_32%\msvcp140_2.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%SYSTEM_32%\msvcp140_atomic_wait.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%SYSTEM_32%\msvcp140_codecvt_ids.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%SYSTEM_32%\ucrtbase.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%SYSTEM_32%\vcruntime140.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%SYSTEM_32%\vcruntime140_1.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")

    # Curl
    plan.copy(r"%CURL_DLLR%\libcurl.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")

    # MongoDB
    plan.copy(r"%MONGO_C_DLLR%\bson-1.0.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%MONGO_C_DLLR%\mongoc-1.0.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%MONGO_CXX_DLLR%\bsoncxx.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%MONGO_CXX_DLLR%\mongocxx.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")

    # OpenCascade
    plan.copy(r"%TBB_DLLR%\tbb12.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%TBB_DLLR%\tbbmalloc.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%JEM_DLLR%\jemalloc.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%OC_DLLR%\TKBRep.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%OC_DLLR%\TKernel.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%OC_DLLR%\TKG2d.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%OC_DLLR%\TKG3d.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%OC_DLLR%\TKGeomAlgo.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%OC_DLLR%\TKGeomBase.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%OC_DLLR%\TKMath.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%OC_DLLR%\TKMesh.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%OC_DLLR%\TKShHealing.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%OC_DLLR%\TKTopAlgo.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")

    # OpenSceneGraph
    plan.copy(r"%OSG_DLLR%\OpenThreads.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%OSG_DLLR%\osg.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%OSG_DLLR%\osgDB.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%OSG_DLLR%\osgGA.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%OSG_DLLR%\osgText.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%OSG_DLLR%\osgUtil.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%OSG_DLLR%\osgViewer.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.mkdir(r"%OPENTWIN_FRONTEND_DEPLOYMENT%\osgPlugins-3.6.3")
    plan.tree(r"%OSG_DLLR%\osgPlugins-3.6.3", r"%OPENTWIN_FRONTEND_DEPLOYMENT%\osgPlugins-3.6.3")

    # OpenSSL
    plan.copy(r"%OPENSSL_DLL%\libcrypto-1_1-x64.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%OPENSSL_DLL%\libssl-1_1-x64.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")

    # Qt
    plan.copy(r"%QT_DLLR%\QT6Core.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%QT_DLLR%\QT6Gui.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%QT_DLLR%\QT6Network.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%QT_DLLR%\QT6OpenGL.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%QT_DLLR%\QT6OpenGLWidgets.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%QT_DLLR%\QT6Svg.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%QT_DLLR%\QT6SvgWidgets.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%QT_DLLR%\QT6WebSockets.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%QT_DLLR%\QT6Widgets.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%QT_DLLR%\QT6Qml.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")

    # Qt AdvancedDockingSystem
    plan.copy(r"%QT_ADS_ROOT%\lib\qtadvanceddocking-qt6.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")

    # Qt plugins
    plan.mkdir(r"%OPENTWIN_FRONTEND_DEPLOYMENT%\plugins")
    plan.mkdir(r"%OPENTWIN_FRONTEND_DEPLOYMENT%\plugins\imageformats")
    plan.copy(r"%QT_PLUGINS%\imageformats\qgif.dll", r"%OPENTWIN_FRONTEND_DEPLOYMENT%\plugins\imageformats")
    plan.copy(r"%QT_PLUGINS%\imageformats\qicns.dll", r"%OPENTWIN_FRONTEND_DEPLOYMENT%\plugins\imageformats")
    plan.copy(r"%QT_PLUGINS%\imageformats\qico.dll", r"%OPENTWIN_FRONTEND_DEPLOYMENT%\plugins\imageformats")
    plan.copy(r"%QT_PLUGINS%\imageformats\qjpeg.dll", r"%OPENTWIN_FRONTEND_DEPLOYMENT%\plugins\imageformats")
    plan.copy(r"%QT_PLUGINS%\imageformats\qsvg.dll", r"%OPENTWIN_FRONTEND_DEPLOYMENT%\plugins\imageformats")
    plan.copy(r"%QT_PLUGINS%\imageformats\qtga.dll", r"%OPENTWIN_FRONTEND_DEPLOYMENT%\plugins\imageformats")
    plan.copy(r"%QT_PLUGINS%\imageformats\qtiff.dll", r"%OPENTWIN_FRONTEND_DEPLOYMENT%\plugins\imageformats")
    plan.copy(r"%QT_PLUGINS%\imageformats\qwbmp.dll", r"%OPENTWIN_FRONTEND_DEPLOYMENT%\plugins\imageformats")
    plan.copy(r"%QT_PLUGINS%\imageformats\qwebp.dll", r"%OPENTWIN_FRONTEND_DEPLOYMENT%\plugins\imageformats")
    plan.mkdir(r"%OPENTWIN_FRONTEND_DEPLOYMENT%\plugins\platforms")
    plan.copy(r"%QT_PLUGINS%\platforms\qwindows.dll", r"%OPENTWIN_FRONTEND_DEPLOYMENT%\plugins\platforms")
    plan.mkdir(r"%OPENTWIN_FRONTEND_DEPLOYMENT%\plugins\tls")
    plan.copy(r"%QT_PLUGINS%\tls\qcertonlybackend.dll", r"%OPENTWIN_FRONTEND_DEPLOYMENT%\plugins\tls")
    plan.copy(r"%QT_PLUGINS%\tls\qopensslbackend.dll", r"%OPENTWIN_FRONTEND_DEPLOYMENT%\plugins\tls")
    plan.copy(r"%QT_PLUGINS%\tls\qschannelbackend.dll", r"%OPENTWIN_FRONTEND_DEPLOYMENT%\plugins\tls")

    # QtTabToolbar
    plan.copy(r"%QT_TT_DLLR%\TabToolbar.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")

    # Qwt
    plan.copy(r"%QWT_LIB_DLLR%\qwt.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")

    # ZLib
    plan.copy(r"%ZLIB_DLLPATHR%\zlib.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")

    # Python
    plan.mkdir(r"%OPENTWIN_FRONTEND_DEPLOYMENT%\PythonEnvironments")
    # plan.copy(r"%OT_PYTHON_ROOT%\python.exe", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%OT_PYTHON_BIN%\Release\python311.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.tree(r"%OT_PYTHON_ROOT%\Environments\CoreEnvironment\Lib\*.*", r"%OPENTWIN_FRONTEND_DEPLOYMENT%\PythonEnvironments\CoreEnvironment\Lib")
    plan.tree(r"%OT_PYTHON_ROOT%\Environments\CoreEnvironment\DLLs\Release\*.*", r"%OPENTWIN_FRONTEND_DEPLOYMENT%\PythonEnvironments\CoreEnvironment\DLLs")

    # Icons
    plan.mkdir(r"%OPENTWIN_FRONTEND_DEPLOYMENT%\icons")
    plan.tree(r"%OPENTWIN_DEV_ROOT%\Assets\Icons", r"%OPENTWIN_FRONTEND_DEPLOYMENT%\icons")

    # ColorStyles
    plan.mkdir(r"%OPENTWIN_FRONTEND_DEPLOYMENT%\colorstyles")
    plan.tree(r"%OPENTWIN_DEV_ROOT%\Assets\ColorStyles", r"%OPENTWIN_FRONTEND_DEPLOYMENT%\colorstyles")

    # GraphicsItems
    plan.mkdir(r"%OPENTWIN_FRONTEND_DEPLOYMENT%\GraphicsItems")
    plan.tree(r"%OPENTWIN_DEV_ROOT%\Assets\GraphicsItems", r"%OPENTWIN_FRONTEND_DEPLOYMENT%\GraphicsItems")

    # Fonts
    plan.mkdir(r"%OPENTWIN_FRONTEND_DEPLOYMENT%\fonts")
    plan.tree(r"%OPENTWIN_DEV_ROOT%\Assets\Fonts", r"%OPENTWIN_FRONTEND_DEPLOYMENT%\fonts")

    # OpenGL Software Rendering
    plan.copy(r"%OPENTWIN_THIRDPARTY_ROOT%\MesaOpenGL\*.*", "%OPENTWIN_FRONTEND_DEPLOYMENT%")

    # OpenTwin
    plan.copy(r"%OT_CADMODELENTITIES_ROOT%\%OT_CDLLR%\OTCADEntities.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%OT_DATASTORAGE_ROOT%\%OT_CDLLR%\OTDataStorage.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%OT_FOUNDATION_ROOT%\%OT_CDLLR%\OTServiceFoundation.dll", "%OPENTWIN_DEPLOYMENT_DIR%")
    plan.copy(r"%OT_LTSPICE_CONNECTOR_ROOT%\%OT_CDLLR%\OTLTSpiceConnector.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%OT_MODELENTITIES_ROOT%\%OT_CDLLR%\OTModelEntities.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%OT_COMMUNICATION_ROOT%\%OT_CDLLR%\OTCommunication.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%OT_MODELAPI_ROOT%\%OT_CDLLR%\OTModelAPI.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%OT_CORE_ROOT%\%OT_CDLLR%\OTCore.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%OT_GUIAPI_ROOT%\%OT_CDLLR%\OTGuiAPI.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%OT_GUI_ROOT%\%OT_CDLLR%\OTGui.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%OT_SYSTEM_ROOT%\%OT_CDLLR%\OTSystem.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%OT_WIDGETS_ROOT%\%OT_CDLLR%\OTWidgets.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%OT_RUBBERBANDAPI_ROOT%\%OT_CDLLR%\OTRubberband.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%OT_RUBBERBAND_OSG_ROOT%\%OT_CDLLR%\OTRubberbandOSG.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%OT_FRONTEND_CONNECTOR_API_ROOT%\%OT_CDLLR%\OTFrontendConnectorAPI.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%OT_FILE_MANAGER_CONNECTOR_ROOT%\%OT_CDLLR%\OTFMC.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%OT_STUDIO_SUITE_CONNECTOR_ROOT%\%OT_CDLLR%\OTStudioSuiteConnector.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%OT_LTSPICE_CONNECTOR_ROOT%\%OT_CDLLR%\OTLTSpiceConnector.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%OT_BLOCKENTITIES_ROOT%\%OT_CDLLR%\OTBlockEntities.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%OT_UICORE_ROOT%\%OT_CDLLR%\uiCore.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%OT_UI_SERVICE_ROOT%\%OT_CDLLR%\uiFrontend.exe", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.rename(r"%OPENTWIN_FRONTEND_DEPLOYMENT%\uiFrontend.exe", "OpenTwin.exe")
    plan.copy(r"%OT_PYTHON_EXECUTION_ROOT%\%OT_CDLLR%\PythonExecution.exe", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%OT_VIEWER_ROOT%\%OT_CDLLR%\OTViewer.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")
    plan.copy(r"%OT_RESULT_DATA_ACCESS_ROOT%\%OT_CDLLR%\OTResultDataAccess.dll", "%OPENTWIN_FRONTEND_DEPLOYMENT%")

    # Certificates
    plan.mkdir(r"%OPENTWIN_FRONTEND_DEPLOYMENT%\Certificates")
    plan.copy(r"%OT_ENCRYPTIONKEY_ROOT%\ca.pem", r"%OPENTWIN_FRONTEND_DEPLOYMENT%\Certificates")
    plan.copy(r"%OT_ENCRYPTIONKEY_ROOT%\certificateKeyFile.pem", r"%OPENTWIN_FRONTEND_DEPLOYMENT%\Certificates")

    # Qt configuration
    plan.write(r"%OPENTWIN_FRONTEND_DEPLOYMENT%\qt.conf", QT_CONF)

    # Build information
    plan.copy(r"%OPENTWIN_DEV_ROOT%\Deployment\BuildInfo.txt", "%OPENTWIN_FRONTEND_DEPLOYMENT%")

    # Installer
    plan.run("creating installer", [MAKENSIS, NSI], INSTALLER_DIR)
    plan.move(INSTALLER_DIR + r"\Install_OpenTwin_Frontend.exe", r"%OPENTWIN_DEV_ROOT%\Deployment\FrontendInstaller")


def create_debug_files(plan) -> None:
    # Shutdown
    plan.action("shutdown")

    # Build
    plan.action("build all")

    # Previous build
    plan.mkdir("%OPENTWIN_DEBUG_FILES%")
    plan.remove(r"%OPENTWIN_DEBUG_FILES%\BlockEditorService.dll")
    plan.remove(r"%OPENTWIN_DEBUG_FILES%\OTSystem.dll")
    plan.remove(r"%OPENTWIN_DEBUG_FILES%\OTCore.dll")
    plan.remove(r"%OPENTWIN_DEBUG_FILES%\OTCommunication.dll")
    plan.remove(r"%OPENTWIN_DEBUG_FILES%\OTServiceFoundation.dll")
    plan.remove(r"%OPENTWIN_DEBUG_FILES%\OTModelEntities.dll")
    plan.remove(r"%OPENTWIN_DEBUG_FILES%\OTCADEntities.dll")
    plan.remove(r"%OPENTWIN_DEBUG_FILES%\OTBlockEntities.dll")
    plan.remove(r"%OPENTWIN_DEBUG_FILES%\Model.dll")
    plan.remove(r"%OPENTWIN_DEBUG_FILES%\OTViewer.dll")
    plan.remove(r"%OPENTWIN_DEBUG_FILES%\OTDataStorage.dll")
    plan.remove(r"%OPENTWIN_DEBUG_FILES%\uiCore.dll")
    plan.remove(r"%OPENTWIN_DEBUG_FILES%\OTRubberband.dll")
    plan.remove(r"%OPENTWIN_DEBUG_FILES%\OTRubberbandOSG.dll")
    plan.remove(r"%OPENTWIN_DEBUG_FILES%\QwtWrapper.dll")
    plan.remove(r"%OPENTWIN_DEBUG_FILES%\GlobalSessionService.dll")
    plan.remove(r"%OPENTWIN_DEBUG_FILES%\LocalSessionService.dll")
    plan.remove(r"%OPENTWIN_DEBUG_FILES%\GlobalDirectoryService.dll")
    plan.remove(r"%OPENTWIN_DEBUG_FILES%\LocalDirectoryService.dll")
    plan.remove(r"%OPENTWIN_DEBUG_FILES%\uiFrontend.exe")
    plan.remove(r"%OPENTWIN_DEBUG_FILES%\RelayService.dll")
    plan.remove(r"%OPENTWIN_DEBUG_FILES%\LoggerService.dll")
    plan.remove(r"%OPENTWIN_DEBUG_FILES%\AuthorisationService.dll")
    plan.remove(r"%OPENTWIN_DEBUG_FILES%\PHREECService.dll")
    plan.remove(r"%OPENTWIN_DEBUG_FILES%\ModelingService.dll")
    plan.remove(r"%OPENTWIN_DEBUG_FILES%\VisualizationService.dll")
    plan.remove(r"%OPENTWIN_DEBUG_FILES%\FITTDService.dll")
    plan.remove(r"%OPENTWIN_DEBUG_FILES%\PythonExecutionService.dll")
    plan.remove(r"%OPENTWIN_DEBUG_FILES%\PythonExecution.exe")
    plan.remove(r"%OPENTWIN_DEBUG_FILES%\ImportParameterizedDataService.dll")
    plan.remove(r"%OPENTWIN_DEBUG_FILES%\DataProcessingService.dll")
    plan.remove(r"%OPENTWIN_DEBUG_FILES%\CartesianMeshService.dll")
    plan.remove(r"%OPENTWIN_DEBUG_FILES%\TetMeshService.dll")
    plan.remove(r"%OPENTWIN_DEBUG_FILES%\GetDPService.dll")
    plan.remove(r"%OPENTWIN_DEBUG_FILES%\ElmerFEMService.dll")
    plan.remove(r"%OPENTWIN_DEBUG_FILES%\StudioSuiteService.dll")
    plan.remove(r"%OPENTWIN_DEBUG_FILES%\LTSpiceService.dll")
    plan.remove(r"%OPENTWIN_DEBUG_FILES%\OTGui.dll")
    plan.remove(r"%OPENTWIN_DEBUG_FILES%\OTGuiAPI.dll")
    plan.remove(r"%OPENTWIN_DEBUG_FILES%\OTWidgets.dll")
    plan.remove(r"%OPENTWIN_DEBUG_FILES%\OTStudioSuiteConnector.dll")
    plan.remove(r"%OPENTWIN_DEBUG_FILES%\OTLTSpiceConnector.dll")
    plan.remove(r"%OPENTWIN_DEBUG_FILES%\open_twin.exe")
    plan.remove(r"%OPENTWIN_DEBUG_FILES%\CircuitSimulatorService.dll")
    plan.remove(r"%OPENTWIN_DEBUG_FILES%\CircuitExecution.exe")
    plan.remove(r"%OPENTWIN_DEBUG_FILES%\OToolkit.dll")
    plan.remove(r"%OPENTWIN_DEBUG_FILES%\OTResultDataAccess.dll")

    # Libraries
    plan.copy(r"%OT_CORE_ROOT%\%OT_CDLLR%\OTCore.dll", "%OPENTWIN_DEBUG_FILES%")
    plan.copy(r"%OT_SYSTEM_ROOT%\%OT_CDLLR%\OTSystem.dll", "%OPENTWIN_DEBUG_FILES%")
    plan.copy(r"%OT_COMMUNICATION_ROOT%\%OT_CDLLR%\OTCommunication.dll", "%OPENTWIN_DEBUG_FILES%")
    plan.copy(r"%OT_FOUNDATION_ROOT%\%OT_CDLLR%\OTServiceFoundation.dll", "%OPENTWIN_DEBUG_FILES%")
    plan.copy(r"%OT_MODELENTITIES_ROOT%\%OT_CDLLR%\OTModelEntities.dll", "%OPENTWIN_DEBUG_FILES%")
    plan.copy(r"%OT_CADMODELENTITIES_ROOT%\%OT_CDLLR%\OTCADEntities.dll", "%OPENTWIN_DEBUG_FILES%")
    plan.copy(r"%OT_VIEWER_ROOT%\%OT_CDLLR%\OTViewer.dll", "%OPENTWIN_DEBUG_FILES%")
    plan.copy(r"%OT_DATASTORAGE_ROOT%\%OT_CDLLR%\OTDataStorage.dll", "%OPENTWIN_DEBUG_FILES%")
    plan.copy(r"%OT_UICORE_ROOT%\%OT_CDLLR%\uiCore.dll", "%OPENTWIN_DEBUG_FILES%")
    plan.copy(r"%OT_RUBBERBANDAPI_ROOT%\%OT_CDLLR%\OTRubberband.dll", "%OPENTWIN_DEBUG_FILES%")
    plan.copy(r"%OT_RUBBERBAND_OSG_ROOT%\%OT_CDLLR%\OTRubberbandOSG.dll", "%OPENTWIN_DEBUG_FILES%")
    plan.copy(r"%OT_QWTWRAPPER_ROOT%\%OT_DLLR%\QwtWrapper.dll", "%OPENTWIN_DEBUG_FILES%")
    plan.copy(r"%OT_GUI_ROOT%\%OT_CDLLR%\OTGui.dll", "%OPENTWIN_DEBUG_FILES%")
    plan.copy(r"%OT_GUIAPI_ROOT%\%OT_CDLLR%\OTGuiAPI.dll", "%OPENTWIN_DEBUG_FILES%")
    plan.copy(r"%OT_WIDGETS_ROOT%\%OT_CDLLR%\OTWidgets.dll", "%OPENTWIN_DEBUG_FILES%")
    plan.copy(r"%OT_STUDIO_SUITE_CONNECTOR_ROOT%\%OT_CDLLR%\OTStudioSuiteConnector.dll", "%OPENTWIN_DEBUG_FILES%")
    plan.copy(r"%OT_LTSPICE_CONNECTOR_ROOT%\%OT_CDLLR%\OTLTSpiceConnector.dll", "%OPENTWIN_DEBUG_FILES%")
    plan.copy(r"%OT_BLOCKENTITIES_ROOT%\%OT_CDLLR%\OTBlockEntities.dll", "%OPENTWIN_DEBUG_FILES%")
    plan.copy(r"%OT_RESULT_DATA_ACCESS_ROOT%\%OT_CDLLR%\OTResultDataAccess.dll", "%OPENTWIN_DEBUG_FILES%")

    # Services
    plan.copy(r"%OT_MODEL_SERVICE_ROOT%\%OT_CDLLR%\Model.dll", "%OPENTWIN_DEBUG_FILES%")
    plan.copy(r"%OT_GLOBAL_SESSION_SERVICE_ROOT%\%OT_CDLLR%\GlobalSessionService.dll", "%OPENTWIN_DEBUG_FILES%")
    plan.copy(r"%OT_LOCAL_SESSION_SERVICE_ROOT%\%OT_CDLLR%\LocalSessionService.dll", "%OPENTWIN_DEBUG_FILES%")
    plan.copy(r"%OT_GLOBAL_DIRECTORY_SERVICE_ROOT%\%OT_CDLLR%\GlobalDirectoryService.dll", "%OPENTWIN_DEBUG_FILES%")
    plan.copy(r"%OT_LOCAL_DIRECTORY_SERVICE_ROOT%\%OT_CDLLR%\LocalDirectoryService.dll", "%OPENTWIN_DEBUG_FILES%")
    plan.copy(r"%OT_UI_SERVICE_ROOT%\%OT_CDLLR%\uiFrontend.exe", "%OPENTWIN_DEBUG_FILES%")
    plan.copy(r"%OT_RELAY_SERVICE_ROOT%\%OT_CDLLR%\RelayService.dll", "%OPENTWIN_DEBUG_FILES%")
    plan.copy(r"%OT_LOGGER_SERVICE_ROOT%\%OT_CDLLR%\LoggerService.dll", "%OPENTWIN_DEBUG_FILES%")
    plan.copy(r"%OT_AUTHORISATION_SERVICE_ROOT%\%OT_CDLLR%\AuthorisationService.dll", "%OPENTWIN_DEBUG_FILES%")
    plan.copy(r"%OT_PHREEC_SERVICE_ROOT%\%OT_CDLLR%\PHREECService.dll", "%OPENTWIN_DEBUG_FILES%")
    # plan.copy(r"%OT_KRIGING_SERVICE_ROOT%\%OT_CDLLR%\KrigingService.dll", "%OPENTWIN_DEBUG_FILES%")
    plan.copy(r"%OT_MODELING_SERVICE_ROOT%\%OT_CDLLR%\ModelingService.dll", "%OPENTWIN_DEBUG_FILES%")
    plan.copy(r"%OT_VISUALIZATION_SERVICE_ROOT%\%OT_CDLLR%\VisualizationService.dll", "%OPENTWIN_DEBUG_FILES%")
    plan.copy(r"%OT_FITTD_SERVICE_ROOT%\%OT_CDLLR%\FITTDService.dll", "%OPENTWIN_DEBUG_FILES%")
    plan.copy(r"%OT_IMPORT_PARAMETERIZED_DATA_SERVICE_ROOT%\%OT_CDLLR%\ImportParameterizedDataService.dll", "%OPENTWIN_DEBUG_FILES%")
    plan.copy(r"%OT_CARTESIAN_MESH_SERVICE_ROOT%\%OT_CDLLR%\CartesianMeshService.dll", "%OPENTWIN_DEBUG_FILES%")
    plan.copy(r"%OT_TET_MESH_SERVICE_ROOT%\%OT_CDLLR%\TetMeshService.dll", "%OPENTWIN_DEBUG_FILES%")
    plan.copy(r"%OT_GETDP_SERVICE_ROOT%\%OT_CDLLR%\GetDPService.dll", "%OPENTWIN_DEBUG_FILES%")
    plan.copy(r"%OT_ELMERFEM_SERVICE_ROOT%\%OT_CDLLR%\ElmerFEMService.dll", "%OPENTWIN_DEBUG_FILES%")
    plan.copy(r"%OT_STUDIOSUITE_SERVICE_ROOT%\%OT_CDLLR%\StudioSuiteService.dll", "%OPENTWIN_DEBUG_FILES%")
    plan.copy(r"%OT_LTSPICE_SERVICE_ROOT%\%OT_CDLLR%\LTSpiceService.dll", "%OPENTWIN_DEBUG_FILES%")
    plan.copy(r"%OT_PYTHON_EXECUTION_SERVICE_ROOT%\%OT_CDLLR%\PythonExecutionService.dll", "%OPENTWIN_DEBUG_FILES%")
    plan.copy(r"%OT_PYTHON_EXECUTION_ROOT%\%OT_CDLLR%\PythonExecution.exe", "%OPENTWIN_DEBUG_FILES%")
    plan.copy(r"%OT_BLOCKEDITORSERVICE_ROOT%\%OT_DLLR%\BlockEditorService.dll", "%OPENTWIN_DEBUG_FILES%")
    plan.copy(r"%OT_DATA_PROCESSING_SERVICE_ROOT%\%OT_CDLLR%\DataProcessingService.dll", "%OPENTWIN_DEBUG_FILES%")
    plan.copy(r"%OT_CIRCUIT_SIMULATOR_SERVICE_ROOT%\%OT_CDLLR%\CircuitSimulatorService.dll", "%OPENTWIN_DEBUG_FILES%")
    plan.copy(r"%OT_CIRCUIT_EXECUTION_ROOT%\%OT_CDLLR%\CircuitExecution.exe", "%OPENTWIN_DEBUG_FILES%")
    plan.copy(r"%OPENTWIN_DEV_ROOT%\Framework\OpenTwin\target\debug\open_twin.exe", "%OPENTWIN_DEBUG_FILES%")

    # Tools
    plan.copy(r"%OPENTWIN_DEV_ROOT%\Tools\OToolkitAPI\%OT_CDLLR%\OToolkitAPI.dll", "%OPENTWIN_DEBUG_FILES%")
    plan.copy(r"%OPENTWIN_DEV_ROOT%\Tools\OToolkit\%OT_CDLLR%\OToolkit.dll", "%OPENTWIN_DEBUG_FILES%")
