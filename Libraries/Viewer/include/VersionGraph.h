#pragma once

#include <qgraphicsview>
#include <qwheelevent>
#include <qscrollbar>

class Viewer;
class VersionGraph;

#include <string>
#include <functional>

class MyGraphicsView : public QGraphicsView
{
	Q_OBJECT
public:
	MyGraphicsView(QWidget *parent = 0) :	QGraphicsView(parent)
	{
		setDragMode(QGraphicsView::ScrollHandDrag);
		setRenderHints(QPainter::Antialiasing | QPainter::SmoothPixmapTransform);
	}

	void resetView(void)
	{
		QRectF boundingRect = scene()->itemsBoundingRect();
		setSceneRect(QRectF());
		int w = boundingRect.width() / 20;
		int h = boundingRect.height() / 20;
		QRectF viewRect = boundingRect.marginsAdded(QMarginsF(w, h, w, h));
		fitInView(boundingRect, Qt::AspectRatioMode::KeepAspectRatio);
		centerOn(boundingRect.center());
	}

	void handleFullView(void)
	{
		QRectF boundingRect = mapFromScene(scene()->itemsBoundingRect()).boundingRect();
		QRect  viewPortRect = viewport()->rect();

		if (viewPortRect.width() > boundingRect.width() && viewPortRect.height() > boundingRect.height())
		{
			resetView();
		}
	}

protected Q_SLOTS:
	void wheelEvent(QWheelEvent *event)
	{
		const ViewportAnchor anchor = transformationAnchor();
		setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
		int angle = event->angleDelta().y();
		qreal factor;
		if (angle > 0) {
			factor = 1.1;
		} else {
			factor = 0.9;
		}
		scale(factor, factor);
		update();

		setTransformationAnchor(anchor);

		handleFullView();
	}

	void mousePressEvent(QMouseEvent *event)
	{
		QGraphicsView::mousePressEvent(event);
		viewport()->setCursor(Qt::CrossCursor);
	}

	void mouseReleaseEvent(QMouseEvent *event)
	{
		QGraphicsView::mouseReleaseEvent(event);
		viewport()->setCursor(Qt::CrossCursor);
	}

	void keyPressEvent(QKeyEvent *event)
	{
		if (event->key() == Qt::Key_Space)
		{
			// Reset the view
			resetView();
		}
	}

	void resizeEvent(QResizeEvent * event)
	{
		QGraphicsView::resizeEvent(event);
		
		handleFullView();
	}

private:

};

class MyGraphicsScene : public QGraphicsScene
{
	Q_OBJECT
public:
	MyGraphicsScene(QWidget *parent = 0) : QGraphicsScene(parent), graph(nullptr) {};

	void setVersionGraph(VersionGraph *_graph) { graph = _graph; }

protected:
	virtual void mouseDoubleClickEvent(QGraphicsSceneMouseEvent *event);


private:
	VersionGraph *graph;
};

class VersionGraphVersion
{
public:
	VersionGraphVersion() : selected(false), rectangle(nullptr), text(nullptr), line(nullptr), offsetX(0), offsetY(0), visible(true), parent(nullptr), activeBranch(false) {};
	VersionGraphVersion(const std::string &_version, const std::string &_description, VersionGraphVersion *_parent) : version(_version), description(_description), parent(_parent), selected(false), rectangle(nullptr), text(nullptr), line(nullptr), offsetX(0), offsetY(0), visible(true), activeBranch(false) {};
	~VersionGraphVersion();

	void setVersion(const std::string &_version) { version = _version; }
	std::string getVersion(void) const { return version; }

	void setDescription(const std::string &_description) { description = _description; }
	std::string getDescription(void) const { return description; }

	void setParent(VersionGraphVersion *_parent) { parent = _parent; }
	VersionGraphVersion *getParent(void) const { return parent; }

	bool isSelected(void) const { return selected; }
	void setSelected(bool _selected) { selected = _selected; }

	bool isVisible(void) const { return visible; }
	void setVisible(bool _visible) { visible = _visible; }

	bool isInActiveBranch(void) const { return activeBranch; }
	void setIsInActiveBranch(bool _active) { activeBranch = _active; };

	void clear(void)
	{
		version.clear();
		description.clear();
		removeChildren();
	}

	VersionGraphVersion *addVersion(const std::string &_version, const std::string &_description)
	{
		VersionGraphVersion *newItem = new VersionGraphVersion(_version, _description, this);
		children.push_back(newItem);
		children.sort([](const VersionGraphVersion *a, const VersionGraphVersion *b) { if (a->getVersion().size() < b->getVersion().size()) return true; if (a->getVersion().size() > b->getVersion().size()) return false; return a->getVersion() < b->getVersion(); });

		return newItem;
	}

	void removeVersion(const std::string &_version)
	{
		for (auto pos = children.begin(); pos != children.end(); pos++)
		{
			if ((*pos)->getVersion() == _version)
			{
				delete *pos;
				children.erase(pos);
				return;
			}
		}
	}
	
	void removeChildrenFromMap(std::map<std::string, VersionGraphVersion *> &_versionMap)
	{
		for (auto child : children)
		{
			_versionMap.erase(child->getVersion());
			child->removeChildrenFromMap(_versionMap);
		}
	}
	
	std::list<VersionGraphVersion *> getChildren(void) { return children; }

	void removeChildren(void) { for (auto child : children) { delete child; }; children.clear(); }

	int getNumberOfBranches(void)
	{
		int numberChildBranches = 0;
		for (auto child : children)
		{
			numberChildBranches += child->getNumberOfBranches();
		}

		return std::max(1, numberChildBranches);
	}

	void removeChildrenFromScene(void);

	void setRectangle(QGraphicsRectItem *_rectangle) { 	rectangle = _rectangle;	}
	void setText(QGraphicsSimpleTextItem *_text) { text = _text; }
	void setLine(QGraphicsLineItem *_line) { line = _line; }

	void setOffset(int _offsetX, int _offsetY) { offsetX = _offsetX; offsetY = _offsetY; }

	QGraphicsRectItem *getRectangle(void) { return rectangle; }
	QGraphicsSimpleTextItem *getText(void) { return text; }
	QGraphicsLineItem *getLine(void) { return line; }

	int getOffsetX(void) { return offsetX; }
	int getOffsetY(void) { return offsetY; }

	void setAutomaticItemVisibility(void);

	void setBranchVisible(bool visible, VersionGraphVersion *&selectedItem);

private:
	std::string version;
	std::string description;
	VersionGraphVersion *parent;
	std::list<VersionGraphVersion *> children;
	bool selected;
	QGraphicsRectItem *rectangle;
	QGraphicsSimpleTextItem *text;
	QGraphicsLineItem *line;
	int offsetX;
	int offsetY;
	bool visible;
	bool activeBranch;
};

class VersionGraph
{
public:
	VersionGraph(Viewer * _viewer);

	virtual ~VersionGraph();

	QWidget * widget(void) const { return m_view; }

	void SetStyleSheets(const std::string & _plotBackgroundSheet, const QColor & _foregroundColor, const QColor & _boxColor, const QColor &_highlightBoxColor, const QColor &_highlightForegroundColor);

	void addVersion(const std::string &_parentVersion, const std::string &_version, const std::string &_description);
	void removeVersion(const std::string &_parentVersion, const std::string &_version);
	void removeVersions(const std::list<std::string> &_versions);

	void selectVersion(const std::string &_version, const std::string &activeBranch);
	
	void updateTree(void);

	void setVersionGraph(std::list<std::tuple<std::string, std::string, std::string>> &versionGraph, const std::string &activeVersion, const std::string &activeBranch);
	
	void clearTree(void);

	void addVersionAndActivate(const std::string &newVersion, const std::string &activeBranch, const std::string &parentVersion, const std::string &_description);
	void itemDoubleClicked(QGraphicsItem *item);

private:
	void drawSubtree(VersionGraphVersion *root, int offsetX, int offsetY, int lineStartX, int lineStartY);
	void drawItem(VersionGraphVersion *item, int offsetX, int offsetY, int lineStartX, int lineStartY);
	void setItemColors(VersionGraphVersion *item);
	void appendNewItem(VersionGraphVersion *parentItem, VersionGraphVersion *item);
	void setAutomaticItemVisibility(void);
	void redrawBranch(VersionGraphVersion *root);
	void changeBranchVisibility(VersionGraphVersion *branchItem, bool makeBranchVisible);
	void ensureSelecteItemAndNeighborsAreVisible(VersionGraphVersion *selectedItem);
	bool addVersionToBranch(const std::string &_version);
	void removeVersionFromBranch(const std::string &_version);
	std::string getBranchFromVersion(const std::string &_version);
	void activateBranch(const std::string &_branch);

	Viewer *						m_viewer;
	MyGraphicsView *				m_view;
	MyGraphicsScene *				m_scene;
	VersionGraphVersion	*		    m_rootVersion;
	std::map<std::string, VersionGraphVersion *> m_versionMap;
	int								m_widthX;
	int								m_widthY;
	int								m_spacingX;
	int								m_spacingY;
	QColor 							m_foregroundColor;
	QColor							m_boxColor;
	QColor							m_boxHighlightColor;
	QColor							m_highlightForegroundColor;
	std::map<std::string, std::string> m_branchTail;
	std::string					    m_activeBranch;
};

