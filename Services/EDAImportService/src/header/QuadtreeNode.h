#ifndef QUADTREENODE_H
#define QUADTREENODE_H

#include <vector>
#include <memory>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepAlgoAPI_Fuse.hxx>

class QuadtreeNode {
public:
    QuadtreeNode(double x, double y, double width, double height);

    void insert(BRepBuilderAPI_MakeFace& face);
    TopoDS_Shape fuse();

private:
    bool contains(BRepBuilderAPI_MakeFace& face);
    void subdivide();
    bool intersects(double qx, double qy, double qwidth, double qheight);
    bool contains_in_query(BRepBuilderAPI_MakeFace& face, double qx, double qy, double qwidth, double qheight);

    std::vector<BRepBuilderAPI_MakeFace> faces;
    std::vector<std::shared_ptr<QuadtreeNode>> children;
    double x, y, width, height;
    bool divided = false;
    static const int max_faces;
    static const double min_size;
};

#endif // QUADTREENODE_H
