#include "ShapeUtilities.h"
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepOffsetAPI_MakeOffset.hxx>
#include <Geom_Plane.hxx>
#include <gp_Pln.hxx>
#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <Geom_Circle.hxx>
#include <gp_Circ.hxx>
#include <BRep_Tool.hxx>

BRepBuilderAPI_MakePolygon ShapeUtilities::makePolygon(std::list<gp_Pnt>& points) {
    BRepBuilderAPI_MakePolygon polygon;
    for (const auto& point : points) {
        polygon.Add(point);
    }
    return polygon;
}

std::list<BRepBuilderAPI_MakePolygon> ShapeUtilities::makePolygons(std::list<std::list<gp_Pnt>>& points) {
    std::list<BRepBuilderAPI_MakePolygon> polygons;
    for (auto& polyPoints : points) {
        polygons.push_back(makePolygon(polyPoints));
    }
    return polygons;
}

BRepBuilderAPI_MakeFace ShapeUtilities::makeFace(BRepBuilderAPI_MakePolygon& polygon) {
    return BRepBuilderAPI_MakeFace(polygon.Wire(), Standard_True);
}

std::list<BRepBuilderAPI_MakeFace> ShapeUtilities::makeFaces(std::list<BRepBuilderAPI_MakePolygon>& polygons, std::list<BRepBuilderAPI_MakeWire>& wires) {
    std::list<BRepBuilderAPI_MakeFace> faces;
    for (auto& polygon : polygons) {
        faces.push_back(makeFace(polygon));
    }
    for (auto& wire : wires) {
        faces.push_back(BRepBuilderAPI_MakeFace(wire.Wire(), Standard_True));
    }
    return faces;
}

BRepBuilderAPI_MakeWire ShapeUtilities::createOffset(std::list<gp_Pnt>& points, int width, int pathType, Standard_Real height) {
    gp_Pnt origin(0.0, 0.0, height);
    gp_Dir normal(0.0, 0.0, 1.0);
    gp_Pln plane(origin, normal);
    //Handle(Geom_Plane) plane = new Geom_Plane(gp::XOY());

    BRepBuilderAPI_MakePolygon mkPolygon;
    for (auto& point : points) {
        mkPolygon.Add(point);
    }
    TopoDS_Face face = BRepBuilderAPI_MakeFace(plane, mkPolygon.Wire(), false);

    BRepOffsetAPI_MakeOffset mkOffset(face, GeomAbs_Intersection, false);
    Standard_Real offset = width / 2.0;
    mkOffset.Perform(offset);

    switch (pathType) {
    case 0: {
        BRepBuilderAPI_MakeWire wireBuilder;
        for (TopExp_Explorer expEdge(mkOffset.Shape(), TopAbs_EDGE); expEdge.More(); expEdge.Next()) {
            TopoDS_Edge edge = TopoDS::Edge(expEdge.Current());
            TopoDS_Vertex v1, v2;
            TopExp::Vertices(edge, v1, v2);
            wireBuilder.Add(BRepBuilderAPI_MakeEdge(BRep_Tool::Pnt(v1), BRep_Tool::Pnt(v2)));
        }
        return wireBuilder;
    }
    case 1: {
        BRepBuilderAPI_MakeWire wireBuilder;
        TopExp_Explorer expWire(mkOffset.Shape(), TopAbs_WIRE);
        wireBuilder.Add(TopoDS::Wire(expWire.Current()));
        return wireBuilder;
    }
    case 2: {
        BRepBuilderAPI_MakeWire wireBuilder;
        for (TopExp_Explorer expEdge(mkOffset.Shape(), TopAbs_EDGE); expEdge.More(); expEdge.Next()) {
            TopoDS_Edge edge = TopoDS::Edge(expEdge.Current());
            Standard_Real first, last;
            Handle(Geom_Curve) curve = BRep_Tool::Curve(edge, first, last);
            if (curve.IsNull()) continue;
            Handle(Geom_Circle) circle = Handle(Geom_Circle)::DownCast(curve);
            if (!circle.IsNull()) {
                gp_Circ circ = circle->Circ();
                gp_Pnt center = circ.Location();
                gp_Pnt pStart = circle->Value(first);
                gp_Pnt pEnd = circle->Value(last);
                gp_Vec pStartCenter(pStart, center);
                gp_Vec vec1(-pStartCenter.Y(), pStartCenter.X(), 0);
                gp_Pnt p1 = pStart.Translated(vec1.Normalized() * offset);
                gp_Vec centerPEnd(center, pEnd);
                gp_Vec vec2(-centerPEnd.Y(), centerPEnd.X(), 0);
                gp_Pnt p2 = pEnd.Translated(vec2.Normalized() * offset);
                wireBuilder.Add(BRepBuilderAPI_MakeEdge(pStart, p1));
                wireBuilder.Add(BRepBuilderAPI_MakeEdge(p1, p2));
                wireBuilder.Add(BRepBuilderAPI_MakeEdge(p2, pEnd));
            }
            else {
                wireBuilder.Add(edge);
            }
        }
        return wireBuilder;
    }
    default:
        return BRepBuilderAPI_MakeWire();
    }
}
