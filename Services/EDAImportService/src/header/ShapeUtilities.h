// @otlicense

#ifndef SHAPEUTILITIES_H
#define SHAPEUTILITIES_H

#include <BRepBuilderAPI_MakePolygon.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <list>
#include <vector>
#include <gp_Pnt.hxx>

class ShapeUtilities {
public:
    static BRepBuilderAPI_MakePolygon makePolygon(std::list<gp_Pnt>& points);
    static std::list<BRepBuilderAPI_MakePolygon> makePolygons(std::list<std::list<gp_Pnt>>& points);
    static BRepBuilderAPI_MakeFace makeFace(BRepBuilderAPI_MakePolygon& polygon);
    static std::list<BRepBuilderAPI_MakeFace> makeFaces(std::list<BRepBuilderAPI_MakePolygon>& polygons, std::list<BRepBuilderAPI_MakeWire>& wires);
    static BRepBuilderAPI_MakeWire createOffset(std::list<gp_Pnt>& points, int width, int pathType, Standard_Real height);
};

#endif // SHAPEUTILITIES_H
