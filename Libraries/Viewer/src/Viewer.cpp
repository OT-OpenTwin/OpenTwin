#include "stdafx.h"

#include "ViewerAPI.h"
#include "Viewer.h"
#include "Model.h"
#include "Notifier.h"
#include "Rubberband.h"
#include "WorkingPlane.h"
#include "AxisCross.h"
#include "AxisCenterCross.h"
#include "ClipPlaneManipulator.h"
#include "TableViewerView.h"

#include "HandlerBase.h"
#include "HandleArrow.h"
#include "TransformManipulator.h"

#include "OTServiceFoundation/SettingsData.h"
#include "OTCore/OTAssert.h"

#include <qobject.h>
#include <qwindow.h>

#include <QtOpenGLWidgets/qopenglwidget.h>
#include <QOpenGLContext>
#include <QOpenGLFunctions>
#include <QMouseEvent>
#include <QWheelEvent>

#include <osgViewer/GraphicsWindow>
#include <osgViewer/Viewer>
#include <osgViewer/CompositeViewer>
#include <osgGA/EventQueue>
#include <osg/Camera>
#include <osg/Material>
#include <osgGA/CameraManipulator>
#include <osgText/Text>
#include <osg/BlendFunc>
#include <osg/ShapeDrawable>
#include <osg/AutoTransform>
#include <osg/MatrixTransform>
#include <osg/LightModel>
#include <osg/PolygonMode>
#include <osg/PolygonOffset>
#include <osg/Depth>
#include <osgUtil/Optimizer>

#include "ViewManipulator.h"
#include "ViewerObjectSelectionHandler.h"

#include "PlotView.h"
#include "VersionGraphView.h"

#include "SceneNodeVTK.h"

#include "ViewerSettings.h"

#include "EntityResultTableData.h"

extern std::string globalFontPath;

Viewer::Viewer(ot::UID modelID, ot::UID viewerID, double sw, double sh, int backgroundR, int backgroundG, int backgroundB, int overlayTextR, int overlayTextG, int overlayTextB) :
	viewerUID(viewerID),
	model(nullptr),
	scaleWidth(sw),
	scaleHeight(sh),
	graphicsWindow(nullptr),
	osgViewer(nullptr),
	leftButtonDown(false),
	middleButtonDown(false),
	rightButtonDown(false),
	selectionHandler(nullptr),
	overlayBox(nullptr),
	overlayText(nullptr),
	rubberband(nullptr),
	rubberbandGroup(nullptr),
	workingPlane(nullptr),
	workingPlaneGroup(nullptr),
	axisCross(nullptr),
	axisCrossGroup(nullptr),
	osgLight(nullptr),
	lightSourceDistance(2.0),
	lightSourceDistanceInfinite(false),
	lastPlotEmpty(true),
	freezeViewFlag(false),
	currentHandler(nullptr),
	clipPlaneActive(false),
	clipPlaneTransform(nullptr),
	clipPlaneManipulator(nullptr)
{
	model = ViewerAPI::getModelFromID(modelID);
	assert(model != nullptr);

	model->attachViewer(this);

	// Create osg viewer 

	setFocusPolicy(Qt::StrongFocus);
	setCursor(QCursor(Qt::CrossCursor));

	setMouseTracking(true);

	// Create new graphics window

	graphicsWindow = new osgViewer::GraphicsWindowEmbedded(this->x(), this->y(), this->width(), this->height());

	// Create new main camera
	float aspectRatio = static_cast<float>(this->width()) / static_cast<float>(this->height());

	osgCamera = new osg::Camera;
	osgCamera->setViewport(0, 0, this->width(), this->height());

	setClearColor(backgroundR, backgroundG, backgroundB, overlayTextR, overlayTextG, overlayTextB);

	osgCamera->setProjectionMatrixAsPerspective(15.f, aspectRatio, 1.f, 10000.f);
	osgCamera->setGraphicsContext(graphicsWindow);

	// Create an overlay camera
	osgOverlayCamera = createOverlayCamera(0, width(), 0, height());

	// Create the overlay text
	overlayFont = osgText::readFontFile(globalFontPath);

	// Create new manipulator
	osgCameraManipulator = new osgGA::ViewManipulator;
	osgCameraManipulator->setAutoComputeHomePosition(true);

	rootNode = new osg::Group;
	osg::Node *geometryRootNode = model->getOSGRootNode();

	clipNode = new osg::ClipNode;
	clipNode->addChild(geometryRootNode);

	rootNode->addChild(clipNode);
	rootNode->addChild(osgOverlayCamera);

	rubberbandTransform = new osg::MatrixTransform;
	rootNode->addChild(rubberbandTransform);

	rubberbandGroup = new osg::Switch;
	rubberbandTransform->addChild(rubberbandGroup);

	workingPlaneGroup = new osg::Group;
	rootNode->addChild(workingPlaneGroup);
	workingPlane = new WorkingPlane(geometryRootNode, workingPlaneGroup);
	workingPlane->setVisible(true);

	axisCrossGroup = new osg::Group;
	axisCross = new AxisCross(axisCrossGroup, overlayFont);
	axisCross->setVisible(true);

	axisCenterCrossGroup = new osg::Group;
	rootNode->addChild(axisCenterCrossGroup);
	axisCenterCross = new AxisCenterCross(geometryRootNode, axisCenterCrossGroup);
	axisCenterCross->setVisible(false);

	handlerNode = new osg::Switch;
	rootNode->addChild(handlerNode);
	handlerNode->getOrCreateStateSet()->setMode(GL_CLIP_PLANE0, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);

	osgLight = new osg::Light;

	osgLight->setLightNum(0);
	osgLight->setAmbient(osg::Vec4(0.5f, 0.5f, 0.5f, 1.0f));
	osgLight->setDiffuse(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	osgLight->setSpecular(osg::Vec4(1.0f, 1.0f, 1.0f, 1.f));
	osgLight->setPosition(osg::Vec4(0.0f, 0.0f, 0.0f, 1.f));

	osg::ref_ptr<osg::LightSource> osgLightSource = new osg::LightSource;
	osgLightSource->setLight(osgLight);

	osg::StateSet* rootStateSet = new osg::StateSet;
	rootNode->setStateSet(rootStateSet);

	rootNode->addChild(osgLightSource.get());
	osgLightSource->setStateSetModes(*(rootNode->getOrCreateStateSet()), osg::StateAttribute::ON);
	
	// Create new viewer
	osgViewer = new osgViewer::Viewer();
	osgViewer->setCamera(osgCamera);
	osgViewer->setSceneData(rootNode);
	//osgViewer->setLight(osgLight);
	osgViewer->setCameraManipulator(osgCameraManipulator);
	osgViewer->setThreadingModel(osgViewer::Viewer::SingleThreaded);
	osgViewer->realize();

	unsigned int clearMask = osgCamera->getClearMask();
	osgCamera->setClearMask(clearMask | GL_STENCIL_BUFFER_BIT);
	osgCamera->setClearStencil(0);

	selectionHandler = new ViewerObjectSelectionHandler(this);
	selectionHandler->setModel(model);

	osgViewer->addEventHandler(selectionHandler);

	// Create a view for the axis cross

	osgAxisCamera = new osg::Camera;

	osgAxisCamera->setViewport(0, 0, this->width()/6, this->height()/6);
	osgAxisCamera->setProjectionMatrixAsPerspective(15.f, aspectRatio, 1.f, 10000.f);
	osgAxisCamera->setGraphicsContext(graphicsWindow);
	osgAxisCamera->setClearMask(GL_DEPTH_BUFFER_BIT );

	osgAxisViewer = new osgViewer::Viewer();

	osg::ref_ptr<osg::Light> osgAxisLight = new osg::Light;

	osgAxisLight->setAmbient(osg::Vec4(0.5f, 0.5f, 0.5f, 1.0f));
	osgAxisLight->setDiffuse(osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f));
	osgAxisLight->setSpecular(osg::Vec4(1.0f, 1.0f, 1.0f, 1.f));

	osgAxisViewer->setCamera(osgAxisCamera);
	osgAxisViewer->setSceneData(axisCrossGroup);
	osgAxisViewer->setThreadingModel(osgViewer::Viewer::SingleThreaded);
	osgAxisViewer->setLight(osgAxisLight);
	osgAxisViewer->realize();

	// Create new Plot
	m_plot = new ot::PlotView(this);

	// Create new view handler
	m_versionGraph = new ot::VersionGraphView(this);
	m_tableViewer = new ot::TableViewerView(this);

	// Create settings
	ot::SettingsData * dataset = createSettings();

	// Load
	getNotifier()->loadSettings(dataset);
	getNotifier()->updateSettings(dataset);
	
	settingsSynchronized(dataset);

	setClearColorAutomatic(backgroundR, backgroundG, backgroundB, overlayTextR, overlayTextG, overlayTextB);

	// Apply Settings
	setLightSourceDistance(ViewerSettings::instance()->geometryLightSourceDistance);

	reset();
}

Viewer::~Viewer()
{
	if (model != nullptr)
	{
		model->detachViewer(this);
	}

	if (selectionHandler != nullptr) delete selectionHandler;
	selectionHandler = nullptr;

	if (workingPlane != nullptr) delete workingPlane;
	workingPlane = nullptr;
	if (axisCross != nullptr) delete axisCross;
	axisCross = nullptr;

	if (m_plot != nullptr) delete m_plot;
	m_plot = nullptr;

	if (m_versionGraph != nullptr) delete m_versionGraph;
	m_versionGraph = nullptr;

	if (m_tableViewer != nullptr) delete m_tableViewer;
	m_tableViewer = nullptr;
}

void Viewer::freeze3DView(bool flag)
{
	freezeViewFlag = flag;

	if (!flag)
	{
		refresh();
	}
}

void Viewer::reset()
{
	if (osgViewer != nullptr)
	{
		updateProjection();
		osgViewer->home();

		osg::Quat rotX;
		rotX.makeRotate(-0.15 * M_PI, osg::Vec3d(1.0, 0.0, 0.0));

		osg::Quat rotZ;
		rotZ.makeRotate(0.25 * M_PI, osg::Vec3d(0.0, 0.0, 1.0));

		osgCameraManipulator->setRotation(osgCameraManipulator->getRotation() * rotX * rotZ);
	}
}

void Viewer::refresh(bool _ignoreUpdateSettingsRequest)
{
	if (freezeViewFlag) return;

	static bool busy = false;
	if (busy)
	{
		assert(0);
		return;
	}
	busy = true;
	
	if (workingPlane) { 
		if (workingPlane->refreshAfterModelChange() && !_ignoreUpdateSettingsRequest) {
			getNotifier()->updateSettings(createSettings());
		}
	}
	if (axisCenterCross) { axisCenterCross->refreshAfterGeometryChange(); }

	if (lastPlotEmpty)
	{
		reset();
	}

	update();

	busy = false;
}

osg::Camera* Viewer::createOverlayCamera(double xmin, double xmax, double ymin, double ymax)
{
	osg::Camera *camera = new osg::Camera;

	camera->setProjectionMatrix(osg::Matrix::ortho2D(xmin, xmax, ymin, ymax));
	camera->setReferenceFrame(osg::Transform::ABSOLUTE_RF);
	camera->setViewMatrix(osg::Matrix::identity());
	camera->setClearMask(GL_DEPTH_BUFFER_BIT);
	camera->setAllowEventFocus(false);
	camera->setRenderOrder(osg::Camera::POST_RENDER);

	return camera;
}

osgText::Text *Viewer::createText(const osg::Vec2 &pos, const std::string &message, float textSize)
{
	osg::Vec3 pos3(pos.x(), pos.y(), 0.0);

	osg::ref_ptr<osgText::Text> text = new osgText::Text;

	text->setFont(overlayFont);
	text->setCharacterSize(textSize);
	text->setFontResolution(120, 120);
	text->setAxisAlignment(osgText::TextBase::XY_PLANE);
	text->setAlignment(osgText::TextBase::AlignmentType::CENTER_CENTER);
	text->setPosition(pos3);
	text->setColor(osg::Vec4(0.0, 0.0, 0.0, 1.0));
	text->setText(message);
	text->setCharacterSizeMode(osgText::TextBase::SCREEN_COORDS);

	return text.release();
}

void Viewer::removeOverlay(void)
{
	if (osgOverlayCamera == nullptr) return;

	while (osgOverlayCamera->removeChild((unsigned int)0));

	overlayBox = nullptr;
	overlayText = nullptr;

	refresh();
}

void Viewer::setOverlayText(const std::string &text)
{
	if (osgOverlayCamera == nullptr) return;

	removeOverlay();

	osg::Geode *textNode = new osg::Geode;

	osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc;
	blendFunc->setFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	const int textHeight = 15;

	overlayBox = new osg::ShapeDrawable;
	overlayBox->setShape(new osg::Box(osg::Vec3(this->width()/2, this->height()- textHeight, -1.0), this->width(), 2* textHeight, 0.0));
	overlayBox->setColor(osg::Vec4(1.0, 0.95, 0.62, 0.8));

	overlayText = createText(osg::Vec2(this->width() / 2, this->height() - textHeight), text, 1.0f * textHeight);

	textNode->addDrawable(overlayBox);
	textNode->addDrawable(overlayText);
	textNode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	textNode->getOrCreateStateSet()->setAttributeAndModes(blendFunc);

	osgOverlayCamera->addChild(textNode);

	refresh();
}

void Viewer::updateOverlaySize(int width, int height)
{
	if (osgOverlayCamera != nullptr)
	{
		osgOverlayCamera->setProjectionMatrix(osg::Matrix::ortho2D(0, width, 0, height));
	}

	const int textHeight = 15;

	if (overlayBox && overlayText)
	{
		overlayText->setPosition(osg::Vec3(width / 2, height - textHeight, 0.0));
		overlayBox->setShape(new osg::Box(osg::Vec3(width / 2, height - textHeight, -1.0), width, 2 * textHeight, 0.0));
	}
}

void Viewer::setClearColorAutomatic(int backgroundR, int backgroundG, int backgroundB, int foregroundR, int foregroundG, int foregroundB)
{
	viewColorAutoBackgroundR = backgroundR;
	viewColorAutoBackgroundG = backgroundG;
	viewColorAutoBackgroundB = backgroundB;

	setClearColorAutomatic();
}

void Viewer::setClearColorAutomatic(void)
{
	if (ViewerSettings::instance()->viewBackgroundColorAutomatic)
	{
		setClearColor(viewColorAutoBackgroundR, viewColorAutoBackgroundG, viewColorAutoBackgroundB, 
				      ViewerSettings::instance()->axisCenterColor.r() * 255, ViewerSettings::instance()->axisCenterColor.g() * 255, ViewerSettings::instance()->axisCenterColor.b() * 255);
	}
	else
	{
		// Set the view background color
		setClearColor(ViewerSettings::instance()->viewBackgroundColor.r() * 255, ViewerSettings::instance()->viewBackgroundColor.g() * 255, ViewerSettings::instance()->viewBackgroundColor.b() * 255, 
					  ViewerSettings::instance()->viewForegroundColor.r() * 255, ViewerSettings::instance()->viewForegroundColor.g() * 255, ViewerSettings::instance()->viewForegroundColor.b() * 255);
	}

	refresh();
}


void Viewer::setClearColor(int backgroundR, int backgroundG, int backgroundB, int overlayTextR, int overlayTextG, int overlayTextB) 
{
	if (osgCamera == nullptr) return; 

	osgCamera->setClearColor(osg::Vec4((float)backgroundR / 255.0, (float)backgroundG / 255.0, (float)backgroundB / 255.0, 1.0f));

	overlayTextColor[0] = (overlayTextR / 255.0);
	overlayTextColor[1] = (overlayTextG / 255.0);
	overlayTextColor[2] = (overlayTextB / 255.0);

	if (axisCross != nullptr) axisCross->setTextColor(overlayTextColor);
}

void Viewer::setLightSourceDistance(const std::string &value)
{
	if (value == "Low")
	{
		lightSourceDistance = 1.15;
		lightSourceDistanceInfinite = false;
	}
	else if (value == "Medium")
	{
		lightSourceDistance = 2.0;
		lightSourceDistanceInfinite = false;
	}
	else if (value == "High")
	{
		lightSourceDistance = 10.0;
		lightSourceDistanceInfinite = false;
	}
	else if (value == "Infinite")
	{
		lightSourceDistance = 10.0;
		lightSourceDistanceInfinite = true;
	}
	else
	{
		assert(0); // Unknown value
	}

	refresh();
}

void Viewer::paintGL()
{
	if (freezeViewFlag) return;

	if (osgViewer != nullptr)
	{
		osgCameraManipulator->updateCamera(*osgCamera);

		// Determine whether the cliplane is oriented towards the viewer
		if (clipPlaneActive)
		{
			osg::Vec3 clipplaneP1 =  clipPlanePoint;
			osg::Vec3 clipplaneP2 =  clipPlanePoint + clipPlaneNormal;

			clipplaneP1 =  clipplaneP1 * osgCamera->getViewMatrix() * osgCamera->getProjectionMatrix();
			clipplaneP2 =  clipplaneP2 * osgCamera->getViewMatrix() * osgCamera->getProjectionMatrix();

			double zDir = clipplaneP2.z() - clipplaneP1.z();

			if (zDir < 0.0)
			{
				// Invert the clipplane
				updateClipPlane(-clipPlaneNormal, clipPlanePoint);
			}
		}

		osg::Vec3f eye, center, up;
		osgCamera->getViewMatrixAsLookAt(eye, center, up);

		eye -= center;
		eye.normalize();

		double size = model->getOSGRootNode()->computeBound().radius();

		if (osgLight != nullptr)
		{		
			osg::Vec3f lightPosition = model->getOSGRootNode()->computeBound().center() + eye * (lightSourceDistance * size);
			osg::Vec4f lightPos4(lightPosition.x(), lightPosition.y(), lightPosition.z(), lightSourceDistanceInfinite ? 0.0 : 1.0);

			osgLight->setPosition(lightPos4);
			osgLight->setDirection(osg::Vec3f(eye.x(), eye.y(), eye.z()));

			if (size <= 0.0)
			{
				osgLight->setConstantAttenuation(1.0f);
				osgLight->setLinearAttenuation(0.0);
			}
			else
			{
				float dynamicLight = 0.1f;
				osgLight->setConstantAttenuation(1 - dynamicLight);
				osgLight->setLinearAttenuation(dynamicLight / (lightSourceDistance * size));
			}
		}

		lastPlotEmpty = (osgCameraManipulator->getDistance() <= 0);

		osgViewer->frame();

		eye *= 12.5;
		center.set(0.0, 0.0, 0.0);
		osgAxisCamera->setViewMatrixAsLookAt(eye, center, up);
		osgAxisViewer->frame();
	}
}

void Viewer::resizeGL(int width, int height)
{
	if (graphicsWindow == nullptr) return;

	getEventQueue()->windowResize(this->x() *scaleWidth, this->y() * scaleHeight, width * scaleWidth, height * scaleHeight);
	graphicsWindow->resized(this->x() * scaleWidth, this->y() * scaleHeight, width * scaleWidth, height * scaleHeight);

	osgCamera->setViewport(0, 0, this->width() * scaleWidth, this->height()* scaleHeight);

	int dx = this->width() / 6 * scaleWidth;
	int dy = this->height() / 6 * scaleHeight;

	int offsetX = 0;
	int offsetY = 0;

	if (dx > dy) offsetX = (dx - dy) / 2;
	if (dy > dx) offsetY = (dy - dx) / 2;

	osgAxisCamera->setViewport(-offsetX, -offsetY, dx, dy);

	updateProjection();

	updateOverlaySize(width, height);
}

void Viewer::updateProjection(void)
{
	int dx = this->width() / 6 * scaleWidth;
	int dy = this->height() / 6 * scaleHeight;

	float aspectRatio = 1.0f * dx / dy;

	osgCamera->setProjectionMatrixAsPerspective(15.f, aspectRatio, 1.f, 10000.f);
	osgAxisCamera->setProjectionMatrixAsPerspective(15.f, aspectRatio, 1.f, 10000.f);
}

void Viewer::initializeGL()
{
	if (osgViewer == nullptr) return;

	osg::Node* rootNode = dynamic_cast <osg::Node*> (osgViewer->getSceneData());

	osg::Material* material = new osg::Material;
	material->setColorMode(osg::Material::AMBIENT_AND_DIFFUSE);
	material->setShininess(osg::Material::FRONT_AND_BACK, 128.0);

	osg::StateSet* stateSet = rootNode->getOrCreateStateSet();
	stateSet->setAttributeAndModes(material, osg::StateAttribute::ON);
	stateSet->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
}

void Viewer::mouseMoveEvent(QMouseEvent* event)
{
	if (graphicsWindow == nullptr) return;

	if (leftButtonDown || middleButtonDown || rightButtonDown)
	{
		model->clearHoverView();
	}

	getEventQueue()->mouseMotion(event->x() * scaleWidth, event->y() * scaleHeight);
}

void Viewer::mouseDoubleClickEvent(QMouseEvent *event)
{
	if (graphicsWindow == nullptr) return;

	unsigned int button = 0;

	switch (event->button())
	{
	case Qt::LeftButton:    button = 1; break;
	case Qt::MiddleButton:  button = 2; break;
	case Qt::RightButton:   button = 3; break;
	default: break;
	}

	getEventQueue()->mouseDoubleButtonPress(event->x() * scaleWidth, event->y() * scaleHeight, button);
}

void Viewer::mousePressEvent(QMouseEvent* event)
{
	if (graphicsWindow == nullptr) return;

	unsigned int button = 0;

	switch (event->button()) 
	{
		case Qt::LeftButton:    button = 1; leftButtonDown = true; break;
		case Qt::MiddleButton:  button = 2; middleButtonDown = true; break;
		case Qt::RightButton:   button = 3; rightButtonDown  = true; break;
		default: break;
	}

	getEventQueue()->mouseButtonPress(event->x() * scaleWidth, event->y() * scaleHeight, button);
}

void Viewer::mouseReleaseEvent(QMouseEvent* event)
{
	if (graphicsWindow == nullptr) return;

	unsigned int button = 0;

	switch (event->button()) 
	{
		case Qt::LeftButton: 	button = 1; leftButtonDown   = false; break;
		case Qt::MiddleButton:  button = 2; middleButtonDown = false; break;
		case Qt::RightButton:   button = 3; rightButtonDown  = false; break;
		default: break;
	}

	getEventQueue()->mouseButtonRelease(event->x() * scaleWidth, event->y() * scaleHeight, button);
}

void Viewer::leaveEvent(QEvent *event)
{
	if (graphicsWindow == nullptr) return;

	selectionHandler->setActive(false);
	model->clearHoverView();
}

void Viewer::enterEvent(QEnterEvent* event)
{
	if (graphicsWindow == nullptr) return;

	selectionHandler->setActive(true);
}

void Viewer::wheelEvent(QWheelEvent* event)
{
	if (graphicsWindow == nullptr) return;
	if (middleButtonDown) return;

	int delta = event->angleDelta().y();
	osgGA::GUIEventAdapter::ScrollingMotion motion = delta > 0 ? osgGA::GUIEventAdapter::SCROLL_DOWN : osgGA::GUIEventAdapter::SCROLL_UP;

	getEventQueue()->mouseScroll(motion);
}

void Viewer::keyPressEvent(QKeyEvent *event)
{
	if (graphicsWindow == nullptr) return;

	if (event->text().isEmpty())
	{
		// Potentially modifier set
		if (event->modifiers() & Qt::ControlModifier)
		{
			getEventQueue()->keyPress(osgGA::GUIEventAdapter::KeySymbol::KEY_Control_L);
		}

		if (event->modifiers() & Qt::ShiftModifier)
		{
			showClipPlaneHandles();
		}
	}
	else
	{
		if (event->key() == Qt::Key_Escape)
		{
			cancelRubberband();
			model->escapeKeyPressed();
		}

		if (event->key() == Qt::Key_Return)
		{
			model->returnKeyPressed();
		}

		if (event->key() == Qt::Key_X && clipPlaneActive)
		{
			updateClipPlane(osg::Vec3(1.0, 0.0, 0.0), clipPlanePoint);
		}

		if (event->key() == Qt::Key_Y && clipPlaneActive)
		{
			updateClipPlane(osg::Vec3(0.0, 1.0, 0.0), clipPlanePoint);
		}

		if (event->key() == Qt::Key_Z && clipPlaneActive)
		{
			updateClipPlane(osg::Vec3(0.0, 0.0, 1.0), clipPlanePoint);
		}

		getEventQueue()->keyPress(event->key());
	}
}

void Viewer::keyReleaseEvent(QKeyEvent *event)
{
	if (graphicsWindow == nullptr) return;

	if (event->key() == Qt::Key_Control)
	{
		getEventQueue()->keyRelease(osgGA::GUIEventAdapter::KeySymbol::KEY_Control_L);
	}
	else if (event->key() == Qt::Key_Shift)
	{
		hideClipPlaneHandles();	
	}
	else
	{
		getEventQueue()->keyRelease(event->key());
	}
}

bool Viewer::event(QEvent* e)
{
	bool handled = false;

	if (freezeViewFlag) return false;

	// Usually the QOpenGLWidget has special treatment for QEvent::WindowChangeInternal, QEvent::ScreenChangeInternal and QEvent::Show
	// The QEvent::WindowChangeInternal and QEvent::ScreenChangeInternal implementation change the rendering context which collides
	// with Open Scene Graph. Therefore we only perform the special handing for QEvent::Show. All other events are handled 
	// by the QWidget implementations
	if (e->type() == QEvent::Show)
	{
		handled = QOpenGLWidget::event(e);
	}
	else
	{
		handled = QWidget::event(e);
	}

	if (e->type() != QEvent::Paint) this->update();

	return handled;
}

osgGA::EventQueue* Viewer::getEventQueue() const
{
	if (graphicsWindow == nullptr) return nullptr;
	
	osgGA::EventQueue* eventQueue = graphicsWindow->getEventQueue();
	return eventQueue;
}

void Viewer::detachFromModel(void)
{
	// Delete the osgViewer
	if (osgViewer == nullptr) return;

	delete osgViewer;

	graphicsWindow			= nullptr;
	osgCamera				= nullptr;
	osgOverlayCamera		= nullptr;
	osgCameraManipulator	= nullptr;
	osgViewer				= nullptr;
	selectionHandler		= nullptr;

	model = nullptr;
}

void Viewer::setTabNames(const std::string & _osgViewTabName, const std::string & _plotTabName, const std::string & _versionGraphTabName) {
	osgViewerTabName = _osgViewTabName;
	m_plotTabName = _plotTabName;
	m_versionGraphTabName = _versionGraphTabName;
}

void Viewer::set1DPlotItemSelected(unsigned long long treeItemID, bool controlPressed)
{
	if (model != nullptr)
	{
		model->set1DPlotItemSelected(treeItemID, controlPressed);
	}
}

void Viewer::reset1DPlotItemSelection(void)
{
	if (model != nullptr)
	{
		model->reset1DPlotItemSelection();
	}
}

void Viewer::createRubberband(ot::serviceID_t _senderId, std::string & _note, const std::string & _configurationJson)
{
	if (rubberband) 
	{
		std::string msg{"Current rubberband (Creator: "};
		msg.append(std::to_string(rubberband->creator())).append("; Note: ").append(rubberband->note()).append(") execution was stopped\n");
		getNotifier()->displayText(msg);
		delete rubberband;
		rubberband = nullptr;
	}

	rubberband = new Rubberband(rubberbandGroup, _senderId, _note, _configurationJson);
	setOverlayText("Double-click left mouse button to select next point or press ESC to cancel");

	setFocus();
}

void Viewer::finishRubberbandExecution(void) 
{
	if (rubberband) 
	{
		osg::Matrix transformMatrix = model->getCurrentWorkingPlaneTransform();
		std::vector<double> transform;
		transform.reserve(16);
		for (int i=0; i < 16; i++)
		{
			transform.push_back(transformMatrix.ptr()[i]);
		}

		getNotifier()->rubberbandFinished(rubberband->creator(), rubberband->note(), rubberband->createPointDataJson(), transform);
		removeOverlay();

		delete rubberband;
		rubberband = nullptr;
	}
	else {
		OTAssert(0, "Finishing execution of not existing rubberband");
	}
}

void Viewer::cancelRubberband(void) {
	if (rubberband) 
	{
		removeOverlay();

		delete rubberband;
		rubberband = nullptr;
	}
}

void Viewer::settingsItemChanged(ot::AbstractSettingsItem * _item) {
	std::string logicalName = _item->logicalName();
	bool updateRequired{ false };
	
	if (workingPlaneSettingsItemChanged(logicalName, _item, updateRequired)) {
		if (workingPlane->refreshAfterSettingsChange() && logicalName != "Viewer:WorkingPlane:GridResolution") { 
			updateRequired = true; 
		}
		refresh(true);
		updateRequired = true;  // We always need to store these settings right away, since they will not be stored later.
	}
	else if (axisCrossSettingsItemChanged(logicalName, _item, updateRequired)) {
		if (axisCross) { axisCross->refreshAfterSettingsChange(); }
		if (axisCenterCross) { axisCenterCross->refreshAfterSettingsChange(); }
		refresh(true);
		updateRequired = true;  // We always need to store these settings right away, since they will not be stored later.
	} 
	else if (geometrySettingsItemChanged(logicalName, _item, updateRequired)) {
		// geometry has to get updated
		refresh(true);
		updateRequired = true;  // We always need to store these settings right away, since they will not be stored later.
	}
	else if (displaySettingsItemChanged(logicalName, _item, updateRequired)) {
		// display has to get updated
		refresh(true);
		updateRequired = true;  // We always need to store these settings right away, since they will not be stored later.
	}
	else {
		OTAssert(0, "Unknown logical name");
	}
	if (updateRequired) {
		ot::SettingsData *settings = createSettings();
		getNotifier()->updateSettings(settings);
		getNotifier()->saveSettings(settings);
	}
}

bool Viewer::workingPlaneSettingsItemChanged(const std::string& _logicalName, ot::AbstractSettingsItem * _item, bool& _settingsUpdateRequired) {
	ViewerSettings * settings = ViewerSettings::instance();

	// Check the name (Group:Subgroup:...:Item)
	if (_logicalName == "Viewer:WorkingPlane:Style") {
		ot::SettingsItemSelection * actualItem = dynamic_cast<ot::SettingsItemSelection *>(_item);
		if (actualItem == nullptr) { OTAssert(0, "Cast item failed"); return true; }
		settings->workingPlaneStyle = actualItem->selectedValue();
		_settingsUpdateRequired = true;
		return true;
	}
	else if (_logicalName == "Viewer:WorkingPlane:DefaultSize") {
		ot::SettingsItemInteger * actualItem = dynamic_cast<ot::SettingsItemInteger *>(_item);
		if (actualItem == nullptr) { OTAssert(0, "Cast item failed"); return true; }
		settings->workingPlaneDefaultSize = actualItem->value();
		return true;
	}
	else if (_logicalName == "Viewer:WorkingPlane:AutoSize") {
		ot::SettingsItemBoolean * actualItem = dynamic_cast<ot::SettingsItemBoolean *>(_item);
		if (actualItem == nullptr) { OTAssert(0, "Cast item failed"); return true; }
		settings->workingPlaneAutoSize = actualItem->value();
		_settingsUpdateRequired = true;
		return true;
	}
	else if (_logicalName == "Viewer:WorkingPlane:Size") {
		ot::SettingsItemInteger * actualItem = dynamic_cast<ot::SettingsItemInteger *>(_item);
		if (actualItem == nullptr) { OTAssert(0, "Cast item failed"); return true; }
		settings->workingPlaneSize = actualItem->value();
		return true;
	}
	else if (_logicalName == "Viewer:WorkingPlane:PlaneColor") {
		ot::SettingsItemColor * actualItem = dynamic_cast<ot::SettingsItemColor *>(_item);
		if (actualItem == nullptr) { OTAssert(0, "Cast item failed"); return true; }
		settings->workingPlanePlaneColor = actualItem->value();
		return true;
	}
	else if (_logicalName == "Viewer:WorkingPlane:GridColor") {
		ot::SettingsItemColor * actualItem = dynamic_cast<ot::SettingsItemColor *>(_item);
		if (actualItem == nullptr) { OTAssert(0, "Cast item failed"); return true; }
		settings->workingPlaneGridLineColor = actualItem->value();
		return true;
	}
	else if (_logicalName == "Viewer:WorkingPlane:GridAutoResolution") {
		ot::SettingsItemBoolean * actualItem = dynamic_cast<ot::SettingsItemBoolean *>(_item);
		if (actualItem == nullptr) { OTAssert(0, "Cast item failed"); return true; }
		settings->workingPlaneAutoGridResolution = actualItem->value();
		_settingsUpdateRequired = true;
		return true;
	}
	else if (_logicalName == "Viewer:WorkingPlane:GridResolution") {
		ot::SettingsItemDouble * actualItem = dynamic_cast<ot::SettingsItemDouble *>(_item);
		if (actualItem == nullptr) { OTAssert(0, "Cast item failed"); return true; }
		settings->workingPlaneGridResolution = actualItem->value();
		return true;
	}
	else if (_logicalName == "Viewer:WorkingPlane:GridHighlightStep") {
		ot::SettingsItemInteger * actualItem = dynamic_cast<ot::SettingsItemInteger *>(_item);
		if (actualItem == nullptr) { OTAssert(0, "Cast item failed"); return true; }
		settings->workingPlaneHighlightEveryStep = actualItem->value();
		return true;
	}
	else if (_logicalName == "Viewer:WorkingPlane:GridLineWidth") {
		ot::SettingsItemInteger * actualItem = dynamic_cast<ot::SettingsItemInteger *>(_item);
		if (actualItem == nullptr) { OTAssert(0, "Cast item failed"); return true; }
		settings->workingPlaneGridLineWidth = actualItem->value();
		return true;
	}
	else if (_logicalName == "Viewer:WorkingPlane:GridWideLineWidth") {
		ot::SettingsItemInteger * actualItem = dynamic_cast<ot::SettingsItemInteger *>(_item);
		if (actualItem == nullptr) { OTAssert(0, "Cast item failed"); return true; }
		settings->workingPlaneWideGridLineWidth = actualItem->value();
		return true;
	}
	return false;
}

bool Viewer::axisCrossSettingsItemChanged(const std::string& _logicalName, ot::AbstractSettingsItem * _item, bool& _settingsUpdateRequired) {
	ViewerSettings * settings = ViewerSettings::instance();

	// Check the name (Group:Subgroup:...:Item)
	if (_logicalName == "Viewer:AxisCross:ColorX") {
		ot::SettingsItemColor * actualItem = dynamic_cast<ot::SettingsItemColor *>(_item);
		if (actualItem == nullptr) { OTAssert(0, "Cast item failed"); return true; }
		settings->axisXColor = actualItem->value();
		return true;
	}
	else if (_logicalName == "Viewer:AxisCross:ColorY") {
		ot::SettingsItemColor * actualItem = dynamic_cast<ot::SettingsItemColor *>(_item);
		if (actualItem == nullptr) { OTAssert(0, "Cast item failed"); return true; }
		settings->axisYColor = actualItem->value();
		return true;
	}
	else if (_logicalName == "Viewer:AxisCross:ColorZ") {
		ot::SettingsItemColor * actualItem = dynamic_cast<ot::SettingsItemColor *>(_item);
		if (actualItem == nullptr) { OTAssert(0, "Cast item failed"); return true; }
		settings->axisZColor = actualItem->value();
		return true;
	}
	else if (_logicalName == "Viewer:AxisCross:ColorCenter") {
		ot::SettingsItemColor * actualItem = dynamic_cast<ot::SettingsItemColor *>(_item);
		if (actualItem == nullptr) { OTAssert(0, "Cast item failed"); return true; }
		settings->axisCenterColor = actualItem->value();
		return true;
	}
	else if (_logicalName == "Viewer:AxisCross:CenterCrossLineWidth") {
		ot::SettingsItemInteger * actualItem = dynamic_cast<ot::SettingsItemInteger *>(_item);
		if (actualItem == nullptr) { OTAssert(0, "Cast item failed"); return true; }
		settings->axisCenterCrossLineWidth = actualItem->value();
		return true;
	}
	else if (_logicalName == "Viewer:AxisCross:DashedLineVisible") {
		ot::SettingsItemBoolean * actualItem = dynamic_cast<ot::SettingsItemBoolean *>(_item);
		if (actualItem == nullptr) { OTAssert(0, "Cast item failed"); return true; }
		settings->axisCenterCrossDashedLineVisible = actualItem->value();
		return true;
	}
	else if (_logicalName == "Viewer:AxisCross:CenterCrossAlswaysAtFront") {
		ot::SettingsItemBoolean * actualItem = dynamic_cast<ot::SettingsItemBoolean *>(_item);
		if (actualItem == nullptr) { OTAssert(0, "Cast item failed"); return true; }
		settings->axisCenterCrossLineAtFront = actualItem->value();
		return true;
	}
	return false;
}

bool Viewer::geometrySettingsItemChanged(const std::string& _logicalName, ot::AbstractSettingsItem *_item, bool& _settingsUpdateRequired) {
	ViewerSettings *settings = ViewerSettings::instance();

	// Check the item (Group:Subgroup:...:Item)
	if (_logicalName == "Viewer:Geometry:HighlightColor") {
		ot::SettingsItemColor *actualItem = dynamic_cast<ot::SettingsItemColor *>(_item);
		if (actualItem == nullptr) {
			OTAssert(0, "Cast item fialed");
			return true;
		}
		settings->geometryHighlightColor = actualItem->value();
		return true;
	}
	else if (_logicalName == "Viewer:Geometry:EdgeColorMode") {
		ot::SettingsItemSelection * actualItem = dynamic_cast<ot::SettingsItemSelection *>(_item);
		if (actualItem == nullptr) {
			OTAssert(0, "Cast item failed");
			return true;
		}
		settings->geometryEdgeColorMode = actualItem->selectedValue();
		_settingsUpdateRequired = true;
		return true;
	}
	else if (_logicalName == "Viewer:Geometry:LightSourceDistance") {
		ot::SettingsItemSelection * actualItem = dynamic_cast<ot::SettingsItemSelection *>(_item);
		if (actualItem == nullptr) {
			OTAssert(0, "Cast item failed");
			return true;
		}
		settings->geometryLightSourceDistance = actualItem->selectedValue();
		setLightSourceDistance(settings->geometryLightSourceDistance);
		return true;
	}
	else if (_logicalName == "Viewer:Geometry:ViewBackgroundColorAutomatic") {
		ot::SettingsItemBoolean * actualItem = dynamic_cast<ot::SettingsItemBoolean *>(_item);
		if (actualItem == nullptr) { OTAssert(0, "Cast item failed"); return true; }
		settings->viewBackgroundColorAutomatic = actualItem->value();
		setClearColorAutomatic();
		_settingsUpdateRequired = true;
		return true;
	}
	else if (_logicalName == "Viewer:Geometry:ViewBackgroundColor") {
		ot::SettingsItemColor *actualItem = dynamic_cast<ot::SettingsItemColor *>(_item);
		if (actualItem == nullptr) {
			OTAssert(0, "Cast item failed");
			return true;
		}
		settings->viewBackgroundColor = actualItem->value();
		setClearColorAutomatic();
		return true;
	}
	else if (_logicalName == "Viewer:Geometry:ViewForegroundColor") {
		ot::SettingsItemColor *actualItem = dynamic_cast<ot::SettingsItemColor *>(_item);
		if (actualItem == nullptr) {
			OTAssert(0, "Cast item failed");
			return true;
		}
		settings->viewForegroundColor = actualItem->value();
		setClearColorAutomatic();
		return true;
	}
	return false;
}

bool Viewer::displaySettingsItemChanged(const std::string& _logicalName, ot::AbstractSettingsItem *_item, bool& _settingsUpdateRequired) {
	ViewerSettings *settings = ViewerSettings::instance();

	// Check the item (Group:Subgroup:...:Item)
	if (_logicalName == "Viewer:Display:UseDisplayList") {
		ot::SettingsItemBoolean * actualItem = dynamic_cast<ot::SettingsItemBoolean *>(_item);
		if (actualItem == nullptr) { OTAssert(0, "Cast item failed"); return true; }
		settings->useDisplayLists = actualItem->value();
		updateDisplaySettings(rootNode);
		_settingsUpdateRequired = true;
		return true;
	}
	else if (_logicalName == "Viewer:Display:UseVertexBuffers") {
		ot::SettingsItemBoolean * actualItem = dynamic_cast<ot::SettingsItemBoolean *>(_item);
		if (actualItem == nullptr) { OTAssert(0, "Cast item failed"); return true; }
		settings->useVertexBufferObjects = actualItem->value();
		updateDisplaySettings(rootNode);
		_settingsUpdateRequired = true;
		return true;
	}

	return false;
}

void Viewer::settingsSynchronized(ot::SettingsData * _dataset) {
	ViewerSettings * settings = ViewerSettings::instance();

	ot::SettingsItemSelection * workingPlaneStyle = dynamic_cast<ot::SettingsItemSelection *>(_dataset->findItemByLogicalName("Viewer:WorkingPlane:Style"));
	ot::SettingsItemInteger * workingPlaneDefaultSize = dynamic_cast<ot::SettingsItemInteger *>(_dataset->findItemByLogicalName("Viewer:WorkingPlane:DefaultSize"));
	ot::SettingsItemBoolean * workingPlaneAutoSize = dynamic_cast<ot::SettingsItemBoolean *>(_dataset->findItemByLogicalName("Viewer:WorkingPlane:AutoSize"));
	ot::SettingsItemInteger * workingPlaneSize = dynamic_cast<ot::SettingsItemInteger *>(_dataset->findItemByLogicalName("Viewer:WorkingPlane:Size"));
	ot::SettingsItemColor * workingPlanePlaneColor = dynamic_cast<ot::SettingsItemColor *>(_dataset->findItemByLogicalName("Viewer:WorkingPlane:PlaneColor"));
	ot::SettingsItemColor * workingPlaneGridColor = dynamic_cast<ot::SettingsItemColor *>(_dataset->findItemByLogicalName("Viewer:WorkingPlane:GridColor"));
	ot::SettingsItemBoolean * workingPlaneGridAutoResolution = dynamic_cast<ot::SettingsItemBoolean *>(_dataset->findItemByLogicalName("Viewer:WorkingPlane:GridAutoResolution"));
	ot::SettingsItemDouble * workingPlaneGridResolution = dynamic_cast<ot::SettingsItemDouble *>(_dataset->findItemByLogicalName("Viewer:WorkingPlane:GridResolution"));
	ot::SettingsItemInteger * workingPlaneGridHighlightStep = dynamic_cast<ot::SettingsItemInteger *>(_dataset->findItemByLogicalName("Viewer:WorkingPlane:GridHighlightStep"));
	ot::SettingsItemInteger * workingPlaneGridLineWidth = dynamic_cast<ot::SettingsItemInteger *>(_dataset->findItemByLogicalName("Viewer:WorkingPlane:GridLineWidth"));
	ot::SettingsItemInteger * workingPlaneGridWideLineWidth = dynamic_cast<ot::SettingsItemInteger *>(_dataset->findItemByLogicalName("Viewer:WorkingPlane:GridWideLineWidth"));

	ot::SettingsItemColor *axisCrossColorX = dynamic_cast<ot::SettingsItemColor *>(_dataset->findItemByLogicalName("Viewer:AxisCross:ColorX"));
	ot::SettingsItemColor *axisCrossColorY = dynamic_cast<ot::SettingsItemColor *>(_dataset->findItemByLogicalName("Viewer:AxisCross:ColorY"));
	ot::SettingsItemColor *axisCrossColorZ = dynamic_cast<ot::SettingsItemColor *>(_dataset->findItemByLogicalName("Viewer:AxisCross:ColorZ"));
	ot::SettingsItemColor *axisCrossColorCenter = dynamic_cast<ot::SettingsItemColor *>(_dataset->findItemByLogicalName("Viewer:AxisCross:ColorCenter"));
	ot::SettingsItemInteger *axisCrossCenterCrossLineWidth = dynamic_cast<ot::SettingsItemInteger *>(_dataset->findItemByLogicalName("Viewer:AxisCross:CenterCrossLineWidth"));
	ot::SettingsItemBoolean *axisCrossDashedLineVisible = dynamic_cast<ot::SettingsItemBoolean *>(_dataset->findItemByLogicalName("Viewer:AxisCross:DashedLineVisible"));
	ot::SettingsItemBoolean *axisCrossCenterCrossAlswaysAtFront = dynamic_cast<ot::SettingsItemBoolean *>(_dataset->findItemByLogicalName("Viewer:AxisCross:CenterCrossAlswaysAtFront"));

	ot::SettingsItemColor *geometryHighlightColor = dynamic_cast<ot::SettingsItemColor *>(_dataset->findItemByLogicalName("Viewer:Geometry:HighlightColor"));
	ot::SettingsItemSelection *geometryEdgeColorMode = dynamic_cast<ot::SettingsItemSelection *>(_dataset->findItemByLogicalName("Viewer:Geometry:EdgeColorMode"));
	ot::SettingsItemSelection *geometryLightSourceDistance = dynamic_cast<ot::SettingsItemSelection *>(_dataset->findItemByLogicalName("Viewer:Geometry:LightSourceDistance"));
	ot::SettingsItemBoolean *geometryViewBackgroundColorAutomatic = dynamic_cast<ot::SettingsItemBoolean *>(_dataset->findItemByLogicalName("Viewer:Geometry:ViewBackgroundColorAutomatic"));
	ot::SettingsItemColor *geometryViewBackgroundColor = dynamic_cast<ot::SettingsItemColor *>(_dataset->findItemByLogicalName("Viewer:Geometry:ViewBackgroundColor"));
	ot::SettingsItemColor *geometryViewForegroundColor = dynamic_cast<ot::SettingsItemColor *>(_dataset->findItemByLogicalName("Viewer:Geometry:ViewForegroundColor"));

	ot::SettingsItemBoolean *useDisplayLists = dynamic_cast<ot::SettingsItemBoolean *>(_dataset->findItemByLogicalName("Viewer:Display:UseDisplayList"));
	ot::SettingsItemBoolean *useVertexBuffers = dynamic_cast<ot::SettingsItemBoolean *>(_dataset->findItemByLogicalName("Viewer:Display:UseVertexBuffers"));

	if (workingPlaneStyle) { settings->workingPlaneStyle = workingPlaneStyle->selectedValue(); } 
	if (workingPlaneDefaultSize) { settings->workingPlaneDefaultSize = workingPlaneDefaultSize->value(); } 
	if (workingPlaneAutoSize) { settings->workingPlaneAutoSize = workingPlaneAutoSize->value(); } 
	if (workingPlaneSize) { settings->workingPlaneSize = workingPlaneSize->value(); } 
	if (workingPlanePlaneColor) { settings->workingPlanePlaneColor = workingPlanePlaneColor->value(); } 
	if (workingPlaneGridColor) { settings->workingPlaneGridLineColor = workingPlaneGridColor->value(); } 
	if (workingPlaneGridAutoResolution) { settings->workingPlaneAutoGridResolution = workingPlaneGridAutoResolution->value(); } 
	if (workingPlaneGridResolution) { settings->workingPlaneGridResolution = workingPlaneGridResolution->value(); } 
	if (workingPlaneGridHighlightStep) { settings->workingPlaneHighlightEveryStep = workingPlaneGridHighlightStep->value(); } 
	if (workingPlaneGridLineWidth) { settings->workingPlaneGridLineWidth = workingPlaneGridLineWidth->value(); } 
	if (workingPlaneGridWideLineWidth) { settings->workingPlaneWideGridLineWidth = workingPlaneGridWideLineWidth->value(); } 

	if (axisCrossColorX) { settings->axisXColor = axisCrossColorX->value(); } 
	if (axisCrossColorY) { settings->axisYColor = axisCrossColorY->value(); } 
	if (axisCrossColorZ) { settings->axisZColor = axisCrossColorZ->value(); } 
	if (axisCrossColorCenter) { settings->axisCenterColor = axisCrossColorCenter->value(); } 
	if (axisCrossCenterCrossLineWidth) { settings->axisCenterCrossLineWidth = axisCrossCenterCrossLineWidth->value(); } 
	if (axisCrossDashedLineVisible) { settings->axisCenterCrossDashedLineVisible = axisCrossDashedLineVisible->value(); } 
	if (axisCrossCenterCrossAlswaysAtFront) { settings->axisCenterCrossLineAtFront = axisCrossCenterCrossAlswaysAtFront->value(); } 

	if (geometryHighlightColor) { settings->geometryHighlightColor = geometryHighlightColor->value(); } 
	if (geometryEdgeColorMode) { settings->geometryEdgeColorMode = geometryEdgeColorMode->selectedValue(); } 
	if (geometryLightSourceDistance) { settings->geometryLightSourceDistance = geometryLightSourceDistance->selectedValue(); } 
	if (geometryViewBackgroundColorAutomatic) { settings->viewBackgroundColorAutomatic = geometryViewBackgroundColorAutomatic->value(); } 
	if (geometryViewBackgroundColor) { settings->viewBackgroundColor = geometryViewBackgroundColor->value(); } 
	if (geometryViewForegroundColor) { settings->viewForegroundColor = geometryViewForegroundColor->value(); } 

	if (useDisplayLists) { settings->useDisplayLists = useDisplayLists->value(); }
	if (useVertexBuffers) { settings->useVertexBufferObjects = useVertexBuffers->value(); }
}

void Viewer::contextMenuItemClicked(const std::string& _menuName, const std::string& _itemName) {

}

void Viewer::contextMenuItemCheckedChanged(const std::string& _menuName, const std::string& _itemName, bool _isChecked) {

}

void Viewer::toggleWorkingPlane(void) {
	if (workingPlane->setVisible(!workingPlane->isVisible())) {
		getNotifier()->updateSettings(createSettings());
	}
}

void Viewer::setWorkingPlaneTransform(osg::Matrix matrix) {
	workingPlaneTransform = matrix;
	matrix.transpose(matrix);
	workingPlane->setTransformation(matrix);
	axisCenterCross->setTransformation(matrix);
	axisCross->setTransformation(matrix);
	rubberbandTransform->setMatrix(matrix);
}

osg::Matrix Viewer::getWorkingPlaneTransform(void)
{
	return workingPlaneTransform;
}

void Viewer::toggleAxisCross(void) {
	axisCross->setVisible(!axisCross->isVisible());
}

void Viewer::toggleCenterAxisCross(void) {
	axisCenterCross->setVisible(!axisCenterCross->isVisible());
}

void Viewer::toggleCutplane(void) {

	if (!clipPlaneActive)
	{
		if (clipPlaneNormal.length() == 0.0)
		{
			// The clip plane had not been activated yet -> initialize it to some defaults
			clipPlaneNormal = osg::Vec3d(0.0, 0.0, 1.0);
			clipPlanePoint = model->getOSGRootNode()->computeBound().center();
		}
		else
		{
			// Check whether the point is inside the bounding sphere
			if (!model->getOSGRootNode()->computeBound().contains(clipPlanePoint))
			{
				clipPlanePoint = model->getOSGRootNode()->computeBound().center();
			}
		}

		enableClipPlane(clipPlaneNormal, clipPlanePoint);
		clipPlaneActive = true;
		setFocus();
	}
	else
	{
		disableClipPlane();
		clipPlaneActive = false;
	}
}

ot::SettingsData * Viewer::createSettings(void) {
	ViewerSettings * settings = ViewerSettings::instance();

	// Create a new dataset
	ot::SettingsData * dataset = new ot::SettingsData("ViewerSettings", "1.0");
	ot::SettingsGroup * view = dataset->addGroup("Viewer", "Viewer");

	// ## Working plane ##
	{
		// Data
		ot::SettingsGroup * workingPlane = view->addSubgroup("WorkingPlane", "Working plane");
		ot::SettingsItemSelection * workingPlaneGridStyle = new ot::SettingsItemSelection("Style", "Grid style", { settings->workingPlaneStyle_Grid, settings->workingPlaneStyle_Plane, settings->workingPlaneStyle_PlaneGrid }, settings->workingPlaneStyle);
		ot::SettingsItemInteger	* workingPlaneDefaultSize = new ot::SettingsItemInteger("DefaultSize", "Default size", settings->workingPlaneDefaultSize, 1, 999999);
		ot::SettingsItemBoolean	* workingPlaneAutoSize = new ot::SettingsItemBoolean("AutoSize", "Automatic size", settings->workingPlaneAutoSize, "If enabled, the working plane will automatically adjust its size according to the current geometry");
		ot::SettingsItemInteger * workingPlaneSize = new ot::SettingsItemInteger("Size", "Size", settings->workingPlaneSize, 1, 999999);
		ot::SettingsItemColor * workingPlaneColor = new ot::SettingsItemColor("PlaneColor", "Plane color", settings->workingPlanePlaneColor);
		ot::SettingsItemColor * workingPlaneGridColor = new ot::SettingsItemColor("GridColor", "Grid color", settings->workingPlaneGridLineColor);
		ot::SettingsItemBoolean * workingPlaneGridAutoResolution = new ot::SettingsItemBoolean("GridAutoResolution", "Grid automatic resolution", settings->workingPlaneAutoGridResolution);
		ot::SettingsItemDouble * workingPlaneGridResolution = new ot::SettingsItemDouble("GridResolution", "Grid resolution", settings->workingPlaneGridResolution, 0.001, 999999, 3);
		ot::SettingsItemInteger * workingPlaneGridHightlightStep = new ot::SettingsItemInteger("GridHighlightStep", "Hightlight grid line", settings->workingPlaneHighlightEveryStep, 0, 100);
		ot::SettingsItemInteger * workingPlaneGridLineWidth = new ot::SettingsItemInteger("GridLineWidth", "Grid line width", settings->workingPlaneGridLineWidth, 1, 20);
		ot::SettingsItemInteger * workingPlaneGridWideLineWidth = new ot::SettingsItemInteger("GridWideLineWidth", "Grid wide line width", settings->workingPlaneWideGridLineWidth, 1, 20);

		// Visibility
		workingPlaneSize->setVisible(!settings->workingPlaneAutoSize);
		workingPlaneColor->setVisible(settings->workingPlaneStyle != settings->workingPlaneStyle_Grid);
		workingPlaneGridColor->setVisible(settings->workingPlaneStyle != settings->workingPlaneStyle_Plane);
		workingPlaneGridResolution->setVisible(!settings->workingPlaneAutoGridResolution);

		// Add items
		workingPlane->addItem(workingPlaneGridStyle);
		workingPlane->addItem(workingPlaneDefaultSize);
		workingPlane->addItem(workingPlaneAutoSize);
		workingPlane->addItem(workingPlaneSize);
		workingPlane->addItem(workingPlaneColor);
		workingPlane->addItem(workingPlaneGridColor);
		workingPlane->addItem(workingPlaneGridAutoResolution);
		workingPlane->addItem(workingPlaneGridResolution);
		workingPlane->addItem(workingPlaneGridHightlightStep);
		workingPlane->addItem(workingPlaneGridLineWidth);
		workingPlane->addItem(workingPlaneGridWideLineWidth);
	}
	// Axis cross
	{
		ot::SettingsGroup * axisCross = view->addSubgroup("AxisCross", "Axis cross");
		ot::SettingsItemColor * axisXColor = new ot::SettingsItemColor("ColorX", "Color X-Axis", settings->axisXColor);
		ot::SettingsItemColor * axisYColor = new ot::SettingsItemColor("ColorY", "Color Y-Axis", settings->axisYColor);
		ot::SettingsItemColor * axisZColor = new ot::SettingsItemColor("ColorZ", "Color Z-Axis", settings->axisZColor);
		ot::SettingsItemColor * axisCenterColor = new ot::SettingsItemColor("ColorCenter", "Center point color", settings->axisCenterColor);
		ot::SettingsItemInteger * axisCenterCrossLineWidth = new ot::SettingsItemInteger("CenterCrossLineWidth", "Line width", settings->axisCenterCrossLineWidth, 1, 50);
		ot::SettingsItemBoolean * axisCenterCrossDashLineVisible = new ot::SettingsItemBoolean("DashedLineVisible", "Negative visible", settings->axisCenterCrossDashedLineVisible);
		ot::SettingsItemBoolean * axisCenterCrossAlwaysFront = new ot::SettingsItemBoolean("CenterCrossAlswaysAtFront", "Keep always at front", settings->axisCenterCrossLineAtFront);

		axisCross->addItem(axisXColor);
		axisCross->addItem(axisYColor);
		axisCross->addItem(axisZColor);
		axisCross->addItem(axisCenterColor);
		axisCross->addItem(axisCenterCrossLineWidth);
		axisCross->addItem(axisCenterCrossDashLineVisible);
		axisCross->addItem(axisCenterCrossAlwaysFront);
	}

	// ## Geometry ##
	{
		// Data
		ot::SettingsGroup *geometry = view->addSubgroup("Geometry", "Geometry");
		ot::SettingsItemColor *geometryHightlightColor = new ot::SettingsItemColor("HighlightColor", "Highlight color", settings->geometryHighlightColor);
		ot::SettingsItemSelection *geometryEdgeColorMode = new ot::SettingsItemSelection("EdgeColorMode", "Edge color mode", { settings->geometryEdgeColorMode_custom, settings->geometryEdgeColorMode_geom, settings->geometryEdgeColorMode_noColor }, settings->geometryEdgeColorMode);
		ot::SettingsItemColor *geometryEdgeColor = new ot::SettingsItemColor("EdgeColor", "Edge color", settings->geometryEdgeColor);
		ot::SettingsItemSelection *geometryLightSourceDistance = new ot::SettingsItemSelection("LightSourceDistance", "Light source distance", { "Low", "Medium", "High", "Infinite" }, settings->geometryLightSourceDistance);
		ot::SettingsItemBoolean * viewBackgroundColorAutomatic = new ot::SettingsItemBoolean("ViewBackgroundColorAutomatic", "Automatic view background color", settings->viewBackgroundColorAutomatic);
		ot::SettingsItemColor *viewBackgroundColor = new ot::SettingsItemColor("ViewBackgroundColor", "View background color", settings->viewBackgroundColor);
		ot::SettingsItemColor *viewForegroundColor = new ot::SettingsItemColor("ViewForegroundColor", "View foreground color", settings->viewForegroundColor);

		// Visibility
		viewBackgroundColor->setVisible(!settings->viewBackgroundColorAutomatic);
		viewForegroundColor->setVisible(!settings->viewBackgroundColorAutomatic);

		// Add items
		geometry->addItem(geometryHightlightColor);
		geometry->addItem(geometryEdgeColorMode);
		geometry->addItem(geometryLightSourceDistance);
		geometry->addItem(viewBackgroundColorAutomatic);
		geometry->addItem(viewBackgroundColor);
		geometry->addItem(viewForegroundColor);
	}

	// ## Display settings ##
	{
		// Data
		ot::SettingsGroup *display = view->addSubgroup("Display", "Display");
		ot::SettingsItemBoolean * useDisplayList = new ot::SettingsItemBoolean("UseDisplayList", "Enable usage of display lists", settings->useDisplayLists);
		ot::SettingsItemBoolean * useVertexBuffers = new ot::SettingsItemBoolean("UseVertexBuffers", "Enable usage of vertex buffer objects", settings->useVertexBufferObjects);

		// Add items
		display->addItem(useDisplayList);
		display->addItem(useVertexBuffers);
	}

	return dataset;
}

void Viewer::addHandler(HandlerBase *handler)
{
	assert(handler != nullptr);
	assert(handler->getNode() != nullptr);

	handlerNode->addChild(handler->getNode());
	handlerList.push_back(handler);
}

void Viewer::removeHandler(HandlerBase *handler)
{
	assert(handler != nullptr);
	assert(handler->getNode() != nullptr);

	handlerList.remove(handler);
	handlerNode->removeChild(handler->getNode());

	if (currentHandler == handler)
	{
		currentHandler = nullptr;
		osgCameraManipulator->setHandleMouseMovement(true);
	}
}

void Viewer::processHoverViewHandlers(osgUtil::LineSegmentIntersector *intersector)
{
	currentHandler = nullptr;

	for (auto item : intersector->getIntersections())
	{
		int nNodes = item.nodePath.size();

		if (nNodes > 0)
		{
			for (int i = nNodes - 1; i >= 0; i--)
			{
				osg::Node *node = item.nodePath[i];

				for (auto handler : handlerList)
				{
					if (node == handler->getNode())
					{
						currentHandler = handler;
						break;
					}
				}

				if (currentHandler != nullptr) break;
			}
		}

		if (currentHandler != nullptr) break;
	}

	for (auto handler : handlerList)
	{
		handler->mouseOver(handler == currentHandler);
	}

	osgCameraManipulator->setHandleMouseMovement(currentHandler == nullptr);
}

bool Viewer::propertyGridValueChanged(const std::string& _groupName, const std::string& _itemName)
{
	return model->propertyGridValueChanged(_groupName, _itemName);
}

void Viewer::enableClipPlane(osg::Vec3d normal, osg::Vec3d point)
{
	clipPlaneNormal = normal;
	clipPlanePoint = point;

	osg::Plane plane(normal, clipPlanePoint);

	osg::ClipPlane *clipPlane = new osg::ClipPlane(0, plane);

	clipNode->addClipPlane(clipPlane);
	clipNode->getOrCreateStateSet()->setMode(GL_CULL_FACE, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);

	createClipPlane(normal, point);
	updateCapGeometry(normal, point);
}

void Viewer::createClipPlane(osg::Vec3d normal, osg::Vec3d point)
{
	osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array(4);
	osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(4);
	osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array(4);

	for (int i{ 0 }; i < 4; i++) {
		colors->at(i).set(0.5, 0.5, 0.5, 0.2);
		normals->at(i).set(0.f, 0.f, 1.f);
	}

	double radius = model->getOSGRootNode()->computeBound().radius();

	vertices->at(0).set(-radius, -radius, 0.0);
	vertices->at(1).set(-radius,  radius, 0.0);
	vertices->at(2).set( radius,  radius, 0.0);
	vertices->at(3).set( radius, -radius, 0.0);

	// Create the geometry object to store the data
	osg::ref_ptr<osg::Geometry> newGeometry = new osg::Geometry;

	if (!ViewerSettings::instance()->useDisplayLists)
	{
		newGeometry->setUseDisplayList(false);
		newGeometry->setUseVertexBufferObjects(ViewerSettings::instance()->useVertexBufferObjects);
	}

	// Enable double sided display
	newGeometry->getOrCreateStateSet()->setMode(GL_CULL_FACE, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
	newGeometry->getOrCreateStateSet()->setAttributeAndModes(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL));

	osg::LightModel *p = new osg::LightModel;
	p->setTwoSided(true);
	newGeometry->getOrCreateStateSet()->setAttributeAndModes(p);

	// Enable polygon offset
	newGeometry->getOrCreateStateSet()->setAttributeAndModes(new osg::PolygonOffset(-1.0f, 1.0f));

	// Now set the vertices, the normal and the colors
	newGeometry->setVertexArray(vertices.get());

	newGeometry->setNormalArray(normals.get());
	newGeometry->setNormalBinding(osg::Geometry::BIND_OVERALL);

	newGeometry->setColorArray(colors.get());
	newGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

	newGeometry->addPrimitiveSet(new osg::DrawArrays(GL_QUADS, 0, 4));

	// Now create the geometry node and assign the drawable
	osg::Geode * nodePlane = new osg::Geode;
	nodePlane->addDrawable(newGeometry);

	osg::StateSet *ssPlane = nodePlane->getOrCreateStateSet();
	ssPlane->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	ssPlane->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
	ssPlane->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
	ssPlane->setMode(GL_CLIP_PLANE0, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);

	osg::ref_ptr<osg::BlendFunc> blendFuncPlane = new osg::BlendFunc;
	blendFuncPlane->setFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	ssPlane->setAttributeAndModes(blendFuncPlane);
	ssPlane->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);

	osg::ref_ptr<osg::Depth> depth = new osg::Depth;
	depth->setWriteMask(false);
	ssPlane->setAttributeAndModes(depth, osg::StateAttribute::ON);
	ssPlane->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);

	ssPlane->setRenderBinDetails(99, "RenderBin"); // draw after everything else

	clipPlaneTransform = new osg::MatrixTransform();
	clipPlaneTransform->addChild(nodePlane);

	osg::Matrix rotateM = osg::Matrix::rotate(osg::Vec3d(0.0, 0.0, 1.0), normal);
	osg::Matrix rotateT = osg::Matrix::translate(clipPlanePoint);

	clipPlaneTransform->setMatrix(rotateM * rotateT);

	clipNode->addChild(clipPlaneTransform);
}

void Viewer::updateClipPlane(osg::Vec3d normal, osg::Vec3d point)
{
	clipPlaneNormal = normal;
	clipPlanePoint = point;

	osg::Plane plane(normal, clipPlanePoint);

	osg::ClipPlane *clipPlane = new osg::ClipPlane(0, plane);

	clipNode->removeClipPlane((unsigned int) 0);
	clipNode->addClipPlane(clipPlane);

	if (clipPlaneTransform != nullptr)
	{
		osg::Matrix rotateM = osg::Matrix::rotate(osg::Vec3d(0.0, 0.0, 1.0), normal);
		osg::Matrix rotateT = osg::Matrix::translate(clipPlanePoint);

		clipPlaneTransform->setMatrix(rotateM * rotateT);
	}

	updateCapGeometry(normal, point);
}

void Viewer::updateCapGeometry(osg::Vec3d normal, osg::Vec3d point)
{
	assert(getModel() != nullptr);

	getModel()->updateCapGeometry(normal, point);
}

void Viewer::disableClipPlane(void)
{
	hideClipPlaneHandles();

	clipNode->removeClipPlane((unsigned int) 0);
	clipNode->getOrCreateStateSet()->removeMode(GL_CULL_FACE);
	clipNode->removeChild(clipPlaneTransform);
}

void Viewer::showClipPlaneHandles(void)
{
	if (clipPlaneManipulator != nullptr) return;

	clipPlaneManipulator = new ClipPlaneManipulator(this, clipPlanePoint, clipPlaneNormal);
}
 
void Viewer::hideClipPlaneHandles(void)
{
	if (clipPlaneManipulator != nullptr)
	{
		delete clipPlaneManipulator;
		clipPlaneManipulator = nullptr;
	}
}

void Viewer::updateDisplaySettings(osg::Node *node)
{
	osg::Group *groupNode = dynamic_cast<osg::Group *>(node);
	osg::Geometry *geometryNode = dynamic_cast<osg::Geometry *>(node);

	if (geometryNode != nullptr)
	{
		if (!ViewerSettings::instance()->useDisplayLists)
		{
			geometryNode->setUseDisplayList(false);
			geometryNode->setUseVertexBufferObjects(ViewerSettings::instance()->useVertexBufferObjects);
		}
		else
		{
			geometryNode->setUseDisplayList(true);
			geometryNode->setUseVertexBufferObjects(false);
			geometryNode->dirtyGLObjects();
		}
	}

	if (groupNode != nullptr)
	{
		// Traverse the children
		for (unsigned int index = 0; index < groupNode->getNumChildren(); index++)
		{
			updateDisplaySettings(groupNode->getChild(index));
		}
	}
}
