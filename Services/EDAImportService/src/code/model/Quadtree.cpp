#include "Quadtree.h"

Quadtree::Quadtree(double x, double y, double width, double height)
    : root(std::make_shared<QuadtreeNode>(x, y, width, height)) {}

void Quadtree::insert(BRepBuilderAPI_MakeFace& face) {
    root->insert(face);
}

void Quadtree::insert(std::list<BRepBuilderAPI_MakeFace>& faces) {
    for (auto& face : faces)
    {
        insert(face);
    }
}

TopoDS_Shape Quadtree::fuse() {
    return root->fuse();
}
