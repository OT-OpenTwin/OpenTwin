#pragma once
#include "stdafx.h"
#include <qgraphicsview>
#include "BlockDiagramEditor.h"
class DrawingView : public QGraphicsView
{
	Q_OBJECT
public:
	DrawingView(QWidget* parent = nullptr) :QGraphicsView(parent)
	{
		_scene = new BlockDiagramEditorScene();
		setScene(_scene);
		_set = true;
	}
	~DrawingView()
	{
		delete _scene;
	}
private:
	bool _set = false;
	BlockDiagramEditorScene * _scene;
};


