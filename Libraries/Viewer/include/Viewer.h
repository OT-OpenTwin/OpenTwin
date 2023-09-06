#pragma once

#include "OpenTwinCore/CoreTypes.h"

#include <QMainWindow>
#include <QOpenGLWidget>

#include "Table.h"
#include "TableViewer.h"

class ViewerObjectSelectionHandler;
class QWidget;
class Model;
class SceneNodeGeometry;
class Rubberband;
class WorkingPlane;
class AxisCross;
class AxisCenterCross;
class HandlerBase;
class ClipPlaneManipulator;

#include <osg/Group>
#include <osg/ClipNode>
#include <osgText/Font>
#include <osgUtil/LineSegmentIntersector>

class ViewerObjectSelectionHandler;
class QWidget;
class Model;
class Plot;
class VersionGraph;
namespace osgGA     { class EventQueue; };
namespace osgViewer { class Viewer; };
namespace osgViewer { class GraphicsWindowEmbedded; };
namespace osg		{ class Camera; };
namespace osgGA		{ class ViewManipulator; };
namespace osgText   { class Text; };
namespace osg		{ class ShapeDrawable; };
namespace osg		{ class Group; }
namespace ot        { class SettingsData; }
namespace ot        { class AbstractSettingsItem; }

class Viewer : public QOpenGLWidget
{
	Q_OBJECT
public:
	Viewer();
	Viewer(ot::UID modelID, ot::UID viewerID, double scaleWidth, double scaleHeight, int backgroundR, int backgroundG, int backgroundB, int overlayTextR, int overlayTextG, int overlayTextB);
	virtual ~Viewer();

	void reset();
	void refresh(bool _ignoreUpdateSettingsRequest = true);
	void setClearColorAutomatic(int backgroundR, int backgroundG, int backgroundB, int foregroundR, int foregroundG, int foregroundB);

	void removeOverlay(void);
	void setOverlayText(const std::string &text);

	ot::UID getViewerID(void) { return viewerUID; };

	void detachFromModel(void);

	Plot * get1DPlot(void) const { return m_plot; }
	VersionGraph * getVersionGraph(void) const { return m_versionGraph; }
	TableViewer* getTableViewer() const { return m_tableViewer; }

	void setTabNames(const std::string & _osgViewTabName, const std::string & _plotTabName, const std::string & _versionGraphTabName);

	void set1DPlotItemSelected(unsigned long long treeItemID, bool controlPressed);
	void reset1DPlotItemSelection(void);

	Rubberband * getRubberband(void) { return rubberband; }

	void createRubberband(ot::serviceID_t _senderId, std::string & _note, const std::string & _configurationJson);

	void finishRubberbandExecution(void);

	void cancelRubberband(void);

	void settingsItemChanged(ot::AbstractSettingsItem * _item);

	bool workingPlaneSettingsItemChanged(const std::string& _logicalName, ot::AbstractSettingsItem * _item, bool& _settingsUpdateRequired);
	bool axisCrossSettingsItemChanged(const std::string& _logicalName, ot::AbstractSettingsItem * _item, bool& _settingsUpdateRequired);
	bool geometrySettingsItemChanged(const std::string& _logicalName, ot::AbstractSettingsItem *_item, bool& _settingsUpdateRequired);
	bool displaySettingsItemChanged(const std::string& _logicalName, ot::AbstractSettingsItem *_item, bool& _settingsUpdateRequired);

	void contextMenuItemClicked(const std::string& _menuName, const std::string& _itemName);

	void contextMenuItemCheckedChanged(const std::string& _menuName, const std::string& _itemName, bool _isChecked);

	void toggleWorkingPlane(void);
	void setWorkingPlaneTransform(osg::Matrix matrix);
	osg::Matrix getWorkingPlaneTransform(void);

	void toggleAxisCross(void);

	void toggleCenterAxisCross(void);

	void toggleCutplane(void);

	void setLightSourceDistance(const std::string &value);
	
	void freeze3DView(bool flag);

	void addHandler(HandlerBase *handler);
	void removeHandler(HandlerBase *handler);

	bool hasHandlers(void) { return !handlerList.empty(); }
	void processHoverViewHandlers(osgUtil::LineSegmentIntersector *intersector);
	HandlerBase *getCurrentHandler(void) { return currentHandler; }

	Model *getModel(void) { return model; }

	bool propertyGridValueChanged(int itemID);

	void enableClipPlane(osg::Vec3d normal, osg::Vec3d point);
	void updateClipPlane(osg::Vec3d normal, osg::Vec3d point);
	void disableClipPlane(void);

	void showClipPlaneHandles(void);
	void hideClipPlaneHandles(void);

protected:
	virtual void paintGL();
	virtual void resizeGL(int width, int height);
	virtual void initializeGL();

	virtual void mouseDoubleClickEvent(QMouseEvent *event);
	virtual void mouseMoveEvent(QMouseEvent *event);
	virtual void mousePressEvent(QMouseEvent *event);
	virtual void mouseReleaseEvent(QMouseEvent *event);

	virtual void leaveEvent(QEvent *event);
	virtual void enterEvent(QEvent * event);

	virtual void wheelEvent(QWheelEvent *event);

	virtual bool event(QEvent *evnt);

	virtual void keyPressEvent(QKeyEvent *event);
	virtual void keyReleaseEvent(QKeyEvent *event) override;

private:
	// Methods
	osgGA::EventQueue* getEventQueue() const;
	osg::Camera* createOverlayCamera(double xmin, double xmax, double ymin, double ymax);
	osgText::Text *createText(const osg::Vec2 &pos, const std::string &text, float textSize);
	void updateOverlaySize(int width, int height);
	ot::SettingsData * createSettings(void);
	void updateProjection(void);
	void createClipPlane(osg::Vec3d normal, osg::Vec3d point);
	void updateCapGeometry(osg::Vec3d normal, osg::Vec3d point);
	void setClearColor(int backgroundR, int backgroundG, int backgroundB, int overlayTextR, int overlayTextG, int overlayTextB);
	void setClearColorAutomatic(void);
	void settingsSynchronized(ot::SettingsData * _dataset);
	void updateDisplaySettings(osg::Node *node);

	// Attributes
	ot::UID							   viewerUID;
	Model							  *model;
	double							   scaleWidth;
	double							   scaleHeight;
	osgViewer::GraphicsWindowEmbedded *graphicsWindow;
	osg::Camera						  *osgCamera;
	osg::Camera						  *osgAxisCamera;
	osg::Camera						  *osgOverlayCamera;
	osgGA::ViewManipulator			  *osgCameraManipulator;
	osgViewer::Viewer				  *osgViewer;
	osgViewer::Viewer				  *osgAxisViewer;
	osg::Light						  *osgLight;
	std::string						   osgViewerTabName;
	bool					           leftButtonDown;
	bool					           middleButtonDown;
	bool					           rightButtonDown;
	ViewerObjectSelectionHandler      *selectionHandler;
	osg::ref_ptr<osg::Group>		   rootNode;
	osg::ref_ptr<osg::Group>		   cutplaneNode;
	osg::ref_ptr<osg::Group>		   viewerNode;
	osg::ref_ptr<osg::ClipNode>		   clipNode;
	osg::ref_ptr<osg::MatrixTransform> clipPlaneTransform;
	osg::ref_ptr<osgText::Font>		   overlayFont;
	double							   overlayTextColor[3];
	osgText::Text					  *overlayText;
	osg::ShapeDrawable			      *overlayBox;
	double							   lightSourceDistance;
	bool							   lightSourceDistanceInfinite;
	osg::ref_ptr<osg::Switch>          handlerNode;
	HandlerBase						  *currentHandler;

	Plot *								m_plot;
	std::string							m_plotTabName;

	VersionGraph *						m_versionGraph;
	TableViewer*						m_tableViewer;

	std::string							m_versionGraphTabName;

	Rubberband *						rubberband;
	osg::ref_ptr<osg::Switch>			rubberbandGroup;
	osg::ref_ptr<osg::MatrixTransform>	rubberbandTransform;

	WorkingPlane *						workingPlane;
	osg::ref_ptr<osg::Group>			workingPlaneGroup;
	osg::Matrix							workingPlaneTransform;

	AxisCross *							axisCross;
	osg::ref_ptr<osg::Group>			axisCrossGroup;

	AxisCenterCross *					axisCenterCross;
	osg::ref_ptr<osg::Group>			axisCenterCrossGroup;

	bool								lastPlotEmpty;
	bool								freezeViewFlag;

	osg::Vec3							clipPlaneNormal;
	osg::Vec3							clipPlanePoint;
	bool								clipPlaneActive;
	ClipPlaneManipulator			   *clipPlaneManipulator;

	std::list<HandlerBase *>			handlerList;

	int viewColorAutoBackgroundR;
	int viewColorAutoBackgroundG;
	int viewColorAutoBackgroundB;
};
