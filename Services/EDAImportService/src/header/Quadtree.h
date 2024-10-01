#ifndef QUADTREE_H
#define QUADTREE_H

#include "QuadtreeNode.h"

class Quadtree {
public:
    Quadtree(double x, double y, double width, double height);
    void insert(std::list<BRepBuilderAPI_MakeFace>& faces);
    TopoDS_Shape fuse();

private:
    std::shared_ptr<QuadtreeNode> root;
    void insert(BRepBuilderAPI_MakeFace& face);
};

#endif // QUADTREE_H
