// @otlicense

#include "IC3DCreator.h"
#include <BRepPrimAPI_MakePrism.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include "Quadtree.h"

void IC3DCreator::createIC(IC& ic, STEPWriter& stepWriter, Viewer& viewer) {
    float old_height = 0;
    float height = 0;
    int colorIndex = 0;

    for (auto& layer : ic.getLayers()) {
        int min_x = 0;
        int max_x = 0;
        int min_y = 0;
        int max_y = 0;
        bool first = true;

        std::list<std::list<gp_Pnt>> pointsOfLayer;

        height += layer.getHeight();
        for (auto& polygon : layer.getPolygons()) {
            std::list<gp_Pnt> pointsOfPolygon;

            for (auto& point : polygon.getCoordinates()) {
                if (first)
                {
                    min_x = point.first;
                    max_x = point.first;
                    min_y = point.second;
                    max_y = point.second;
                    first = false;
                } else
                {
                    min_x = min(point.first, min_x);
                    max_x = max(point.first, max_x);
                    min_y = min(point.second, min_y);
                    max_y = max(point.second, max_y);
                }
                pointsOfPolygon.push_back(gp_Pnt(point.first, point.second, height));
            }
            pointsOfLayer.push_back(pointsOfPolygon);
        }

        std::list<BRepBuilderAPI_MakeWire> wires;
        for (auto& path : layer.getPaths()) {
            std::list<gp_Pnt> pointsOfPath;

            for (auto& point : path.getCoordinates()) {
                if (first)
                {
                    min_x = point.first;
                    max_x = point.first;
                    min_y = point.second;
                    max_y = point.second;
                    first = false;
                }
                else
                {
                    min_x = min(point.first, min_x);
                    max_x = max(point.first, max_x);
                    min_y = min(point.second, min_y);
                    max_y = max(point.second, max_y);
                }
                pointsOfPath.push_back(gp_Pnt(point.first, point.second, height));
            }

            wires.push_back(ShapeUtilities::createOffset(pointsOfPath, path.getWidth(), path.getPathType(), height));
        }
        auto polygons = ShapeUtilities::makePolygons(pointsOfLayer);
        auto faces = ShapeUtilities::makeFaces(polygons, wires);
        TopoDS_Shape shape;
        if (faces.size() == 1) {
            shape = BRepPrimAPI_MakePrism(faces.front().Shape(), gp_Vec(0.0, 0.0, layer.getThikness()));
        }
        else if (faces.size() > 1) {
            Quadtree quadtree(min_x, min_y, max_x - min_x, max_y - min_y);
            quadtree.insert(faces);
            shape = quadtree.fuse();
            shape = BRepPrimAPI_MakePrism(shape, gp_Vec(0.0, 0.0, layer.getThikness()));
        } else
        {
            shape = TopoDS_Shape();
        }

        TopoDS_Compound compound;
        BRep_Builder builder;
        builder.MakeCompound(compound);
        builder.Add(compound, shape);

        stepWriter.addShape(shape,colorIndex, layer.getId(), true);

        // create substrate
        BRepBuilderAPI_MakePolygon sub_polygon(
            gp_Pnt(min_x, min_y, old_height),
            gp_Pnt(min_x, max_y, old_height),
            gp_Pnt(max_x, max_y, old_height),
            gp_Pnt(max_x, min_y, old_height));
        sub_polygon.Add(gp_Pnt(min_x, min_y, old_height));
        auto sub_face = ShapeUtilities::makeFace(sub_polygon);
        BRepPrimAPI_MakePrism sub_prism(sub_face, gp_Vec(0.0, 0.0, height-old_height));
        TopoDS_Shape sub_shape = sub_prism.Shape();
        stepWriter.addShape(sub_shape, colorIndex, layer.getId(), false);
        colorIndex++;
        old_height = height;

        viewer << shape << sub_shape;
    }
}
