#include "stdafx.h"

#include "ViewerAPI.h"
#include "Viewer.h"
#include "Model.h"
#include "FrontendAPI.h"
#include "Rubberband.h"
#include "WorkingPlane.h"
#include "AxisCross.h"
#include "GlobalFontPath.h"
#include "AxisCenterCross.h"
#include "ClipPlaneManipulator.h"

#include "HandlerBase.h"
#include "HandleArrow.h"
#include "TransformManipulator.h"

#include "OTCore/OTAssert.h"
#include "OTGui/Property.h"
#include "OTGui/PropertyInt.h"
#include "OTGui/PropertyBool.h"
#include "OTGui/PropertyColor.h"
#include "OTGui/PropertyDouble.h"
#include "OTGui/PropertyString.h"
#include "OTGui/PropertyStringList.h"
#include "OTGui/PropertyGroup.h"
#include "OTWidgets/GlobalColorStyle.h"

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

#include "PlotManager.h"
#include "PlotManagerView.h"

#include "SceneNodeVTK.h"

#include "ViewerSettings.h"

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
	mouseCursorText(nullptr),
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
	clipPlaneManipulator(nullptr),
	overlayTextNode(nullptr),
	mouseCursorX(0.0),
	mouseCursorY(0.0),
	freezeWorkingPlane(false)
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
	overlayFont = osgText::readFontFile(GlobalFontPath::instance());

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
	m_plot = new ot::PlotManagerView;

	// Create settings
	ot::PropertyGridCfg dataset = createSettings();

	// Load
	FrontendAPI::instance()->loadSettings(dataset);
	FrontendAPI::instance()->updateSettings(dataset);
	
	settingsSynchronized(dataset);

	setClearColorAutomatic(backgroundR, backgroundG, backgroundB, overlayTextR, overlayTextG, overlayTextB);

	// Apply Settings
	setLightSourceDistance(ViewerSettings::instance()->geometryLightSourceDistance);

	reset();

	this->connect(&ot::GlobalColorStyle::instance(), &ot::GlobalColorStyle::currentStyleChanged, this, &Viewer::slotColorStyleChanged);
}

void Viewer::slotColorStyleChanged(void) 
{
	setClearColorAutomatic();
	refresh();
}

void Viewer::slotUpdateViewerSettings(void) {
	ot::PropertyGridCfg settings = createSettings();
	FrontendAPI::instance()->updateSettings(settings);
	FrontendAPI::instance()->saveSettings(settings);
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
	
	if (!getFreezeWorkingPlane())
	{
		if (workingPlane) {
			if (workingPlane->refreshAfterModelChange() && !_ignoreUpdateSettingsRequest) {
				FrontendAPI::instance()->updateSettings(createSettings());
			}
		}
		if (axisCenterCross) { axisCenterCross->refreshAfterGeometryChange(); }
	}

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

	while (osgOverlayCamera->removeChild(overlayTextNode));

	overlayBox = nullptr;
	overlayText = nullptr;

	refresh();
}

void Viewer::setOverlayText(const std::string &text)
{
	if (osgOverlayCamera == nullptr) return;

	removeOverlay();

	overlayTextNode = new osg::Geode;

	osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc;
	blendFunc->setFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	const int textHeight = 15;

	overlayBox = new osg::ShapeDrawable;
	overlayBox->setShape(new osg::Box(osg::Vec3(this->width()/2, this->height()- textHeight, -1.0), this->width(), 2* textHeight, 0.0));
	overlayBox->setColor(osg::Vec4(1.0, 0.95, 0.62, 0.8));

	overlayText = createText(osg::Vec2(this->width() / 2, this->height() - textHeight), text, 1.0f * textHeight);

	overlayTextNode->addDrawable(overlayBox);
	overlayTextNode->addDrawable(overlayText);
	overlayTextNode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
	overlayTextNode->getOrCreateStateSet()->setAttributeAndModes(blendFunc);

	osgOverlayCamera->addChild(overlayTextNode);

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
		ot::ColorStyleValue backColor = ot::GlobalColorStyle::instance().getCurrentStyle().getValue(ot::ColorStyleValueEntry::WindowBackground);
		ot::ColorStyleValue frontColor = ot::GlobalColorStyle::instance().getCurrentStyle().getValue(ot::ColorStyleValueEntry::WindowForeground);

		viewColorAutoBackgroundR = backColor.toColor().red();
		viewColorAutoBackgroundG = backColor.toColor().green();
		viewColorAutoBackgroundB = backColor.toColor().blue();

		setClearColor(viewColorAutoBackgroundR, viewColorAutoBackgroundG, viewColorAutoBackgroundB, 
			          frontColor.toColor().red(), frontColor.toColor().green(), frontColor.toColor().blue());
	}
	else
	{
		// Set the view background color
		setClearColor(ViewerSettings::instance()->viewBackgroundColor.r(), ViewerSettings::instance()->viewBackgroundColor.g(), ViewerSettings::instance()->viewBackgroundColor.b(), 
					  ViewerSettings::instance()->viewForegroundColor.r(), ViewerSettings::instance()->viewForegroundColor.g(), ViewerSettings::instance()->viewForegroundColor.b());
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

	mouseCursorX = event->x() * scaleWidth;
	mouseCursorY = event->y() * scaleHeight;
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
		FrontendAPI::instance()->displayText(msg);
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

		FrontendAPI::instance()->rubberbandFinished(rubberband->creator(), rubberband->note(), rubberband->createPointDataJson(), transform);
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

void Viewer::settingsItemChanged(const ot::Property* _item) {
	std::string logicalName = _item->getPropertyPath();
	bool updateRequired{ false };
	
	if (workingPlaneSettingsItemChanged(logicalName, _item, updateRequired)) {
		if (workingPlane->refreshAfterSettingsChange() && logicalName != "Working Plane/Grid Resolution") { 
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
		QMetaObject::invokeMethod(this, &Viewer::slotUpdateViewerSettings, Qt::QueuedConnection);
	}
}

bool Viewer::workingPlaneSettingsItemChanged(const std::string& _logicalName, const ot::Property* _item, bool& _settingsUpdateRequired) {
	ViewerSettings * settings = ViewerSettings::instance();

	// Check the name (Group:Subgroup:...:Item)
	if (_logicalName == "Working Plane/Grid Style") {
		const ot::PropertyStringList * actualItem = dynamic_cast<const ot::PropertyStringList *>(_item);
		if (actualItem == nullptr) { OTAssert(0, "Cast item failed"); return true; }
		settings->workingPlaneStyle = actualItem->getCurrent();
		_settingsUpdateRequired = true;
		return true;
	}
	else if (_logicalName == "Working Plane/Default Size") {
		const ot::PropertyInt * actualItem = dynamic_cast<const ot::PropertyInt *>(_item);
		if (actualItem == nullptr) { OTAssert(0, "Cast item failed"); return true; }
		settings->workingPlaneDefaultSize = actualItem->getValue();
		return true;
	}
	else if (_logicalName == "Working Plane/Automatic Size") {
		const ot::PropertyBool * actualItem = dynamic_cast<const ot::PropertyBool *>(_item);
		if (actualItem == nullptr) { OTAssert(0, "Cast item failed"); return true; }
		settings->workingPlaneAutoSize = actualItem->getValue();
		_settingsUpdateRequired = true;
		return true;
	}
	else if (_logicalName == "Working Plane/Size") {
		const ot::PropertyInt * actualItem = dynamic_cast<const ot::PropertyInt *>(_item);
		if (actualItem == nullptr) { OTAssert(0, "Cast item failed"); return true; }
		settings->workingPlaneSize = actualItem->getValue();
		return true;
	}
	else if (_logicalName == "Working Plane/Plane Color") {
		const ot::PropertyColor * actualItem = dynamic_cast<const ot::PropertyColor *>(_item);
		if (actualItem == nullptr) { OTAssert(0, "Cast item failed"); return true; }
		settings->workingPlanePlaneColor = actualItem->getValue();
		return true;
	}
	else if (_logicalName == "Working Plane/Grid Color") {
		const ot::PropertyColor * actualItem = dynamic_cast<const ot::PropertyColor *>(_item);
		if (actualItem == nullptr) { OTAssert(0, "Cast item failed"); return true; }
		settings->workingPlaneGridLineColor = actualItem->getValue();
		return true;
	}
	else if (_logicalName == "Working Plane/Automatic Grid Resolution") {
		const ot::PropertyBool * actualItem = dynamic_cast<const ot::PropertyBool *>(_item);
		if (actualItem == nullptr) { OTAssert(0, "Cast item failed"); return true; }
		settings->workingPlaneAutoGridResolution = actualItem->getValue();
		_settingsUpdateRequired = true;
		return true;
	}
	else if (_logicalName == "Working Plane/Grid Resolution") {
		const ot::PropertyDouble * actualItem = dynamic_cast<const ot::PropertyDouble *>(_item);
		if (actualItem == nullptr) { OTAssert(0, "Cast item failed"); return true; }
		settings->workingPlaneGridResolution = actualItem->getValue();
		return true;
	}
	else if (_logicalName == "Working Plane/Highlight Grid Line") {
		const ot::PropertyInt * actualItem = dynamic_cast<const ot::PropertyInt *>(_item);
		if (actualItem == nullptr) { OTAssert(0, "Cast item failed"); return true; }
		settings->workingPlaneHighlightEveryStep = actualItem->getValue();
		return true;
	}
	else if (_logicalName == "Working Plane/Grid Line Width") {
		const ot::PropertyInt * actualItem = dynamic_cast<const ot::PropertyInt *>(_item);
		if (actualItem == nullptr) { OTAssert(0, "Cast item failed"); return true; }
		settings->workingPlaneGridLineWidth = actualItem->getValue();
		return true;
	}
	else if (_logicalName == "Working Plane/Wide Grid Line Width") {
		const ot::PropertyInt * actualItem = dynamic_cast<const ot::PropertyInt *>(_item);
		if (actualItem == nullptr) { OTAssert(0, "Cast item failed"); return true; }
		settings->workingPlaneWideGridLineWidth = actualItem->getValue();
		return true;
	}
	return false;
}

bool Viewer::axisCrossSettingsItemChanged(const std::string& _logicalName, const ot::Property* _item, bool& _settingsUpdateRequired) {
	ViewerSettings * settings = ViewerSettings::instance();

	// Check the name (Group:Subgroup:...:Item)
	if (_logicalName == "Axis Cross/Color X-Axis") {
		const ot::PropertyColor * actualItem = dynamic_cast<const ot::PropertyColor *>(_item);
		if (actualItem == nullptr) { OTAssert(0, "Cast item failed"); return true; }
		settings->axisXColor = actualItem->getValue();
		return true;
	}
	else if (_logicalName == "Axis Cross/Color Y-Axis") {
		const ot::PropertyColor * actualItem = dynamic_cast<const ot::PropertyColor *>(_item);
		if (actualItem == nullptr) { OTAssert(0, "Cast item failed"); return true; }
		settings->axisYColor = actualItem->getValue();
		return true;
	}
	else if (_logicalName == "Axis Cross/Color Z-Axis") {
		const ot::PropertyColor * actualItem = dynamic_cast<const ot::PropertyColor *>(_item);
		if (actualItem == nullptr) { OTAssert(0, "Cast item failed"); return true; }
		settings->axisZColor = actualItem->getValue();
		return true;
	}
	else if (_logicalName == "Axis Cross/Center Point Color") {
		const ot::PropertyColor * actualItem = dynamic_cast<const ot::PropertyColor *>(_item);
		if (actualItem == nullptr) { OTAssert(0, "Cast item failed"); return true; }
		settings->axisCenterColor = actualItem->getValue();
		return true;
	}
	else if (_logicalName == "Axis Cross/Line Width") {
		const ot::PropertyInt * actualItem = dynamic_cast<const ot::PropertyInt *>(_item);
		if (actualItem == nullptr) { OTAssert(0, "Cast item failed"); return true; }
		settings->axisCenterCrossLineWidth = actualItem->getValue();
		return true;
	}
	else if (_logicalName == "Axis Cross/Negative Visible") {
		const ot::PropertyBool * actualItem = dynamic_cast<const ot::PropertyBool *>(_item);
		if (actualItem == nullptr) { OTAssert(0, "Cast item failed"); return true; }
		settings->axisCenterCrossDashedLineVisible = actualItem->getValue();
		return true;
	}
	else if (_logicalName == "Axis Cross/Keep At Front") {
		const ot::PropertyBool * actualItem = dynamic_cast<const ot::PropertyBool *>(_item);
		if (actualItem == nullptr) { OTAssert(0, "Cast item failed"); return true; }
		settings->axisCenterCrossLineAtFront = actualItem->getValue();
		return true;
	}
	return false;
}

bool Viewer::geometrySettingsItemChanged(const std::string& _logicalName, const ot::Property* _item, bool& _settingsUpdateRequired) {
	ViewerSettings *settings = ViewerSettings::instance();

	// Check the item (Group:Subgroup:...:Item)
	if (_logicalName == "Geometry/Highlight Color") {
		const ot::PropertyColor *actualItem = dynamic_cast<const ot::PropertyColor *>(_item);
		if (actualItem == nullptr) {
			OTAssert(0, "Cast item fialed");
			return true;
		}
		settings->geometryHighlightColor = actualItem->getValue();
		return true;
	}
	else if (_logicalName == "Geometry/Edge Color Mode") {
		const ot::PropertyStringList * actualItem = dynamic_cast<const ot::PropertyStringList *>(_item);
		if (actualItem == nullptr) {
			OTAssert(0, "Cast item failed");
			return true;
		}
		settings->geometryEdgeColorMode = actualItem->getCurrent();
		_settingsUpdateRequired = true;
		return true;
	}
	else if (_logicalName == "Geometry/Light Source Distance") {
		const ot::PropertyStringList * actualItem = dynamic_cast<const ot::PropertyStringList *>(_item);
		if (actualItem == nullptr) {
			OTAssert(0, "Cast item failed");
			return true;
		}
		settings->geometryLightSourceDistance = actualItem->getCurrent();
		setLightSourceDistance(settings->geometryLightSourceDistance);
		return true;
	}
	else if (_logicalName == "Geometry/Automatic View Background Color") {
		const ot::PropertyBool * actualItem = dynamic_cast<const ot::PropertyBool *>(_item);
		if (actualItem == nullptr) { OTAssert(0, "Cast item failed"); return true; }
		settings->viewBackgroundColorAutomatic = actualItem->getValue();
		setClearColorAutomatic();
		_settingsUpdateRequired = true;
		return true;
	}
	else if (_logicalName == "Geometry/View Background Color") {
		const ot::PropertyColor *actualItem = dynamic_cast<const ot::PropertyColor *>(_item);
		if (actualItem == nullptr) {
			OTAssert(0, "Cast item failed");
			return true;
		}
		settings->viewBackgroundColor = actualItem->getValue();
		setClearColorAutomatic();
		return true;
	}
	else if (_logicalName == "Geometry/View Foreground Color") {
		const ot::PropertyColor *actualItem = dynamic_cast<const ot::PropertyColor *>(_item);
		if (actualItem == nullptr) {
			OTAssert(0, "Cast item failed");
			return true;
		}
		settings->viewForegroundColor = actualItem->getValue();
		setClearColorAutomatic();
		return true;
	}
	return false;
}

bool Viewer::displaySettingsItemChanged(const std::string& _logicalName, const ot::Property* _item, bool& _settingsUpdateRequired) {
	ViewerSettings *settings = ViewerSettings::instance();

	// Check the item (Group:Subgroup:...:Item)
	if (_logicalName == "Display/Enable Display Lists") {
		const ot::PropertyBool * actualItem = dynamic_cast<const ot::PropertyBool *>(_item);
		if (actualItem == nullptr) { OTAssert(0, "Cast item failed"); return true; }
		settings->useDisplayLists = actualItem->getValue();
		updateDisplaySettings(rootNode);
		_settingsUpdateRequired = true;
		return true;
	}
	else if (_logicalName == "Display/Enable Vertex Buffer") {
		const ot::PropertyBool * actualItem = dynamic_cast<const ot::PropertyBool *>(_item);
		if (actualItem == nullptr) { OTAssert(0, "Cast item failed"); return true; }
		settings->useVertexBufferObjects = actualItem->getValue();
		updateDisplaySettings(rootNode);
		_settingsUpdateRequired = true;
		return true;
	}

	return false;
}

void Viewer::settingsSynchronized(const ot::PropertyGridCfg& _dataset) {
	ViewerSettings * settings = ViewerSettings::instance();

	using namespace ot;

	const ot::PropertyStringList * workingPlaneStyle = dynamic_cast<const ot::PropertyStringList *>(_dataset.findPropertyByPath("Working Plane/Grid Style"));
	const ot::PropertyInt * workingPlaneDefaultSize = dynamic_cast<const ot::PropertyInt *>(_dataset.findPropertyByPath("Working Plane/Default Size"));
	const ot::PropertyBool * workingPlaneAutoSize = dynamic_cast<const ot::PropertyBool *>(_dataset.findPropertyByPath("Working Plane/Automatic Size"));
	const ot::PropertyInt * workingPlaneSize = dynamic_cast<const ot::PropertyInt *>(_dataset.findPropertyByPath("Working Plane/Size"));
	const ot::PropertyColor * workingPlanePlaneColor = dynamic_cast<const ot::PropertyColor *>(_dataset.findPropertyByPath("Working Plane/Plane Color"));
	const ot::PropertyColor * workingPlaneGridColor = dynamic_cast<const ot::PropertyColor *>(_dataset.findPropertyByPath("Working Plane/Grid Color"));
	const ot::PropertyBool * workingPlaneGridAutoResolution = dynamic_cast<const ot::PropertyBool *>(_dataset.findPropertyByPath("Working Plane/Automatic Grid Resolution"));
	const ot::PropertyDouble * workingPlaneGridResolution = dynamic_cast<const ot::PropertyDouble *>(_dataset.findPropertyByPath("Working Plane/Grid Resolution"));
	const ot::PropertyInt * workingPlaneGridHighlightStep = dynamic_cast<const ot::PropertyInt *>(_dataset.findPropertyByPath("Working Plane/Highlight Grid Line"));
	const ot::PropertyInt * workingPlaneGridLineWidth = dynamic_cast<const ot::PropertyInt *>(_dataset.findPropertyByPath("Working Plane/Grid Line Width"));
	const ot::PropertyInt * workingPlaneGridWideLineWidth = dynamic_cast<const ot::PropertyInt *>(_dataset.findPropertyByPath("Working Plane/Wide Grid Line Width"));

	const ot::PropertyColor *axisCrossColorX = dynamic_cast<const ot::PropertyColor *>(_dataset.findPropertyByPath("Axis Cross/Color X-Axis"));
	const ot::PropertyColor *axisCrossColorY = dynamic_cast<const ot::PropertyColor *>(_dataset.findPropertyByPath("Axis Cross/Color Y-Axis"));
	const ot::PropertyColor *axisCrossColorZ = dynamic_cast<const ot::PropertyColor *>(_dataset.findPropertyByPath("Axis Cross/Color Z-Axis"));
	const ot::PropertyColor *axisCrossColorCenter = dynamic_cast<const ot::PropertyColor *>(_dataset.findPropertyByPath("Axis Cross/Center Point Color"));
	const ot::PropertyInt *axisCrossCenterCrossLineWidth = dynamic_cast<const ot::PropertyInt *>(_dataset.findPropertyByPath("Axis Cross/Line Width"));
	const ot::PropertyBool *axisCrossDashedLineVisible = dynamic_cast<const ot::PropertyBool *>(_dataset.findPropertyByPath("Axis Cross/Negative Visible"));
	const ot::PropertyBool *axisCrossCenterCrossAlswaysAtFront = dynamic_cast<const ot::PropertyBool *>(_dataset.findPropertyByPath("Axis Cross/Keep At Front"));

	const ot::PropertyColor *geometryHighlightColor = dynamic_cast<const ot::PropertyColor *>(_dataset.findPropertyByPath("Geometry/Highlight Color"));
	const ot::PropertyStringList *geometryEdgeColorMode = dynamic_cast<const ot::PropertyStringList *>(_dataset.findPropertyByPath("Geometry/Edge Color Mode"));
	const ot::PropertyStringList *geometryLightSourceDistance = dynamic_cast<const ot::PropertyStringList *>(_dataset.findPropertyByPath("Geometry/Light Source Distance"));
	const ot::PropertyBool *geometryViewBackgroundColorAutomatic = dynamic_cast<const ot::PropertyBool *>(_dataset.findPropertyByPath("Geometry/Automatic View Background Color"));
	const ot::PropertyColor *geometryViewBackgroundColor = dynamic_cast<const ot::PropertyColor *>(_dataset.findPropertyByPath("Geometry/View Background Color"));
	const ot::PropertyColor *geometryViewForegroundColor = dynamic_cast<const ot::PropertyColor *>(_dataset.findPropertyByPath("Geometry/View Foreground Color"));

	const ot::PropertyBool *useDisplayLists = dynamic_cast<const ot::PropertyBool *>(_dataset.findPropertyByPath("Display/Enable Display Lists"));
	const ot::PropertyBool *useVertexBuffers = dynamic_cast<const ot::PropertyBool *>(_dataset.findPropertyByPath("Display/Enable Vertex Buffer"));

	if (workingPlaneStyle) { settings->workingPlaneStyle = workingPlaneStyle->getCurrent(); } 
	if (workingPlaneDefaultSize) { settings->workingPlaneDefaultSize = workingPlaneDefaultSize->getValue(); } 
	if (workingPlaneAutoSize) { settings->workingPlaneAutoSize = workingPlaneAutoSize->getValue(); }
	if (workingPlaneSize) { settings->workingPlaneSize = workingPlaneSize->getValue(); }
	if (workingPlanePlaneColor) { settings->workingPlanePlaneColor = workingPlanePlaneColor->getValue(); }
	if (workingPlaneGridColor) { settings->workingPlaneGridLineColor = workingPlaneGridColor->getValue(); }
	if (workingPlaneGridAutoResolution) { settings->workingPlaneAutoGridResolution = workingPlaneGridAutoResolution->getValue(); }
	if (workingPlaneGridResolution) { settings->workingPlaneGridResolution = workingPlaneGridResolution->getValue(); }
	if (workingPlaneGridHighlightStep) { settings->workingPlaneHighlightEveryStep = workingPlaneGridHighlightStep->getValue(); }
	if (workingPlaneGridLineWidth) { settings->workingPlaneGridLineWidth = workingPlaneGridLineWidth->getValue(); }
	if (workingPlaneGridWideLineWidth) { settings->workingPlaneWideGridLineWidth = workingPlaneGridWideLineWidth->getValue(); }

	if (axisCrossColorX) { settings->axisXColor = axisCrossColorX->getValue(); }
	if (axisCrossColorY) { settings->axisYColor = axisCrossColorY->getValue(); }
	if (axisCrossColorZ) { settings->axisZColor = axisCrossColorZ->getValue(); }
	if (axisCrossColorCenter) { settings->axisCenterColor = axisCrossColorCenter->getValue(); }
	if (axisCrossCenterCrossLineWidth) { settings->axisCenterCrossLineWidth = axisCrossCenterCrossLineWidth->getValue(); }
	if (axisCrossDashedLineVisible) { settings->axisCenterCrossDashedLineVisible = axisCrossDashedLineVisible->getValue(); }
	if (axisCrossCenterCrossAlswaysAtFront) { settings->axisCenterCrossLineAtFront = axisCrossCenterCrossAlswaysAtFront->getValue(); }

	if (geometryHighlightColor) { settings->geometryHighlightColor = geometryHighlightColor->getValue(); }
	if (geometryEdgeColorMode) { settings->geometryEdgeColorMode = geometryEdgeColorMode->getCurrent(); }
	if (geometryLightSourceDistance) { settings->geometryLightSourceDistance = geometryLightSourceDistance->getCurrent(); }
	if (geometryViewBackgroundColorAutomatic) { settings->viewBackgroundColorAutomatic = geometryViewBackgroundColorAutomatic->getValue(); }
	if (geometryViewBackgroundColor) { settings->viewBackgroundColor = geometryViewBackgroundColor->getValue(); }
	if (geometryViewForegroundColor) { settings->viewForegroundColor = geometryViewForegroundColor->getValue(); }

	if (useDisplayLists) { settings->useDisplayLists = useDisplayLists->getValue(); }
	if (useVertexBuffers) { settings->useVertexBufferObjects = useVertexBuffers->getValue(); }
}

void Viewer::toggleWorkingPlane(void) {
	if (workingPlane->setVisible(!workingPlane->isVisible())) {
		FrontendAPI::instance()->updateSettings(createSettings());
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

ot::PropertyGridCfg Viewer::createSettings(void) {
	ViewerSettings * settings = ViewerSettings::instance();

	using namespace ot;

	PropertyGridCfg config;

	// ## Working plane ##
	{
		// Data
		PropertyGroup* workingPlane = new ot::PropertyGroup("Working Plane");
		PropertyStringList* workingPlaneGridStyle = new PropertyStringList("Grid Style", settings->workingPlaneStyle, std::list<std::string>({ settings->workingPlaneStyle_Grid, settings->workingPlaneStyle_Plane, settings->workingPlaneStyle_PlaneGrid }));
		PropertyInt* workingPlaneDefaultSize = new PropertyInt("Default Size", settings->workingPlaneDefaultSize, 1, 999999);
		PropertyBool* workingPlaneAutoSize = new PropertyBool("Automatic Size", settings->workingPlaneAutoSize);
		workingPlaneAutoSize->setPropertyTip("If enabled, the working plane will automatically adjust its size according to the current geometry");
		PropertyInt* workingPlaneSize = new PropertyInt("Size", settings->workingPlaneDefaultSize, 1, 999999);
		PropertyColor* workingPlaneColor = new PropertyColor("Plane Color", settings->workingPlanePlaneColor);
		PropertyColor* workingPlaneGridColor = new PropertyColor("Grid Color", settings->workingPlaneGridLineColor);
		PropertyBool* workingPlaneGridAutoResolution = new PropertyBool("Automatic Grid Resolution", settings->workingPlaneAutoGridResolution);
		PropertyDouble* workingPlaneGridResolution = new PropertyDouble("Grid Resolution", settings->workingPlaneGridResolution, 0.001, 999999);
		workingPlaneGridResolution->setPrecision(3);
		PropertyInt* workingPlaneGridHightlightStep = new PropertyInt("Highlight Grid Line", settings->workingPlaneHighlightEveryStep, 0, 100);
		PropertyInt* workingPlaneGridLineWidth = new PropertyInt("Grid Line Width", settings->workingPlaneGridLineWidth, 1, 20);
		PropertyInt* workingPlaneGridWideLineWidth = new PropertyInt("Wide Grid Line Width", settings->workingPlaneWideGridLineWidth, 1, 20);

		// Visibility
		workingPlaneSize->setPropertyFlag(Property::IsHidden, settings->workingPlaneAutoSize);
		workingPlaneColor->setPropertyFlag(Property::IsHidden, settings->workingPlaneStyle == settings->workingPlaneStyle_Grid);
		workingPlaneGridColor->setPropertyFlag(Property::IsHidden, settings->workingPlaneStyle == settings->workingPlaneStyle_Plane);
		workingPlaneGridResolution->setPropertyFlag(Property::IsHidden, settings->workingPlaneAutoGridResolution);

		// Add items
		workingPlane->addProperty(workingPlaneGridStyle);
		workingPlane->addProperty(workingPlaneDefaultSize);
		workingPlane->addProperty(workingPlaneAutoSize);
		workingPlane->addProperty(workingPlaneSize);
		workingPlane->addProperty(workingPlaneColor);
		workingPlane->addProperty(workingPlaneGridColor);
		workingPlane->addProperty(workingPlaneGridAutoResolution);
		workingPlane->addProperty(workingPlaneGridResolution);
		workingPlane->addProperty(workingPlaneGridHightlightStep);
		workingPlane->addProperty(workingPlaneGridLineWidth);
		workingPlane->addProperty(workingPlaneGridWideLineWidth);

		config.addRootGroup(workingPlane);
	}
	// Axis cross
	{
		PropertyGroup* axisCross = new ot::PropertyGroup("Axis Cross");
		PropertyColor* axisXColor = new PropertyColor("Color X-Axis", settings->axisXColor);
		PropertyColor* axisYColor = new PropertyColor("Color Y-Axis", settings->axisYColor);
		PropertyColor* axisZColor = new PropertyColor("Color Z-Axis", settings->axisZColor);
		PropertyColor* axisCenterColor = new PropertyColor("Center Point Color", settings->axisCenterColor);
		PropertyInt* axisCenterCrossLineWidth = new PropertyInt("Line Width", settings->axisCenterCrossLineWidth, 1, 50);
		PropertyBool* axisCenterCrossDashLineVisible = new PropertyBool("Negative Visible", settings->axisCenterCrossDashedLineVisible);
		PropertyBool* axisCenterCrossAlwaysFront = new PropertyBool("Keep At Front", settings->axisCenterCrossLineAtFront);
		
		axisCross->addProperty(axisXColor);
		axisCross->addProperty(axisYColor);
		axisCross->addProperty(axisZColor);
		axisCross->addProperty(axisCenterColor);
		axisCross->addProperty(axisCenterCrossLineWidth);
		axisCross->addProperty(axisCenterCrossDashLineVisible);
		axisCross->addProperty(axisCenterCrossAlwaysFront);

		config.addRootGroup(axisCross);
	}

	// ## Geometry ##
	{
		// Data#
		PropertyGroup* geometry = new ot::PropertyGroup("Geometry");
		PropertyColor* geometryHightlightColor = new PropertyColor("Highlight Color", settings->geometryHighlightColor);
		PropertyStringList* geometryEdgeColorMode = new PropertyStringList("Edge Color Mode", settings->geometryEdgeColorMode, std::list<std::string>({ settings->geometryEdgeColorMode_custom, settings->geometryEdgeColorMode_geom, settings->geometryEdgeColorMode_noColor }));
		PropertyColor* geometryEdgeColor = new PropertyColor("Edge Color", settings->geometryEdgeColor);
		PropertyStringList* geometryLightSourceDistance = new PropertyStringList("Light Source Distance", settings->geometryLightSourceDistance, std::list<std::string>({ "Low", "Medium", "High", "Infinite" }));
		PropertyBool* viewBackgroundColorAutomatic = new PropertyBool("Automatic View Background Color", settings->viewBackgroundColorAutomatic);
		PropertyColor* viewBackgroundColor = new PropertyColor("View Background Color", settings->viewBackgroundColor);
		PropertyColor* viewForegroundColor = new PropertyColor("View Foreground Color", settings->viewForegroundColor);
	
		// Visibility
		viewBackgroundColor->setPropertyFlag(Property::IsHidden, settings->viewBackgroundColorAutomatic);
		viewForegroundColor->setPropertyFlag(Property::IsHidden, settings->viewBackgroundColorAutomatic);

		// Add items
		geometry->addProperty(geometryHightlightColor);
		geometry->addProperty(geometryEdgeColorMode);
		geometry->addProperty(geometryLightSourceDistance);
		geometry->addProperty(viewBackgroundColorAutomatic);
		geometry->addProperty(viewBackgroundColor);
		geometry->addProperty(viewForegroundColor);

		config.addRootGroup(geometry);
	}

	// ## Display settings ##
	{
		// Data
		PropertyGroup* display = new ot::PropertyGroup("Display");
		PropertyBool* useDisplayList = new PropertyBool("Enable Display Lists", settings->useDisplayLists);
		useDisplayList->setPropertyTip("Enables the usage of display lists");
		PropertyBool* useVertexBuffers = new PropertyBool("Enable Vertex Buffer", settings->useVertexBufferObjects);
		useVertexBuffers->setPropertyTip("Enables the usage of vertex buffer objects");

		// Add items
		display->addProperty(useDisplayList);
		display->addProperty(useVertexBuffers);

		config.addRootGroup(display);
	}

	return config;
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

bool Viewer::propertyGridValueChanged(const ot::Property* _property)
{
	return model->propertyGridValueChanged(_property);
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

void Viewer::setCursorText(const std::string& text)
{
	if (text == cursorText) return;
	cursorText = text;

	if (osgOverlayCamera == nullptr) return;

	while (osgOverlayCamera->removeChild(cursorTextNode));

	cursorTextNode = nullptr;
	mouseCursorBox = nullptr;

	if (!text.empty())
	{
		cursorTextNode = new osg::Geode;

		osg::ref_ptr<osg::BlendFunc> blendFunc = new osg::BlendFunc;
		blendFunc->setFunction(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		const int textHeight = 15;

		float centerX = mouseCursorX / scaleWidth;
		float centerY = this->height() - mouseCursorY / scaleHeight - 1.3 * textHeight;

		mouseCursorText = createText(osg::Vec2(centerX, centerY), text, 1.0f * textHeight);

		osg::BoundingBox textBox = mouseCursorText->computeBoundingBox();

		mouseCursorBox = new osg::ShapeDrawable;
		mouseCursorBox->setShape(new osg::Box(osg::Vec3(centerX, centerY, -1.0), 1.2 * (textBox.xMax() - textBox.xMin()), 1.2 * textHeight, 0.0));
		mouseCursorBox->setColor(osg::Vec4(1.0, 1.0, 1.0, 0.6));

		cursorTextNode->addDrawable(mouseCursorBox);
		cursorTextNode->addDrawable(mouseCursorText);
		cursorTextNode->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
		cursorTextNode->getOrCreateStateSet()->setAttributeAndModes(blendFunc);

		osgOverlayCamera->addChild(cursorTextNode);

		refresh();
	}
}
