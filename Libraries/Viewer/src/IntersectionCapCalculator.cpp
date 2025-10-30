// @otlicense
// File: IntersectionCapCalculator.cpp
// 
// License:
// Copyright 2025 by OpenTwin
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// @otlicense-end

#include "stdafx.h"

#include <osg/Node>
#include <osg/Geode>
#include <osg/Transform>
#include <osg/LightModel>
#include <osg/PolygonMode>
#include <osg/PolygonOffset>
#include <osg/LineWidth>

#include "IntersectionCapCalculator.h"

#include "SceneNodeGeometry.h"
#include "SceneNodeMaterial.h"
#include "ViewerSettings.h"
#include "TextureMapManager.h"

#include <fstream>
#include <iostream>
#include <vector>
#include <cmath>
#include <optional>
#include <map>
#include <array>
#include <set>
#include <unordered_map>
#include <cassert>

#include "earcut.hpp"

namespace mapbox {
    namespace util {
        template <> struct nth<0, IntersectionCapCalculatorVec2> {
            static auto get(const IntersectionCapCalculatorVec2& t) { return t.x; }
        };
        template <> struct nth<1, IntersectionCapCalculatorVec2> {
            static auto get(const IntersectionCapCalculatorVec2& t) { return t.y; }
        };
    }
}


IntersectionCapCalculator::IntersectionCapCalculator()
{

}

IntersectionCapCalculator::~IntersectionCapCalculator()
{

}

IntersectionCapCalculatorProjection2D IntersectionCapCalculator::makeProjectionBasis(const IntersectionCapCalculatorPlane& plane) {
    IntersectionCapCalculatorVec3 n = plane.normal.normalized();
    IntersectionCapCalculatorVec3 u = (fabs(n.z) < 0.9 ? n.cross({ 0,0,1 }) : n.cross({ 1,0,0 })).normalized();
    IntersectionCapCalculatorVec3 v = n.cross(u).normalized();
    return { plane.point, u, v };
}

std::optional<IntersectionCapCalculatorVec3> IntersectionCapCalculator::intersectEdge(const IntersectionCapCalculatorVec3& a, const IntersectionCapCalculatorVec3& b, double da, double db) {
    if ((da >= 0 && db < 0) || (da < 0 && db >= 0)) {
        double t = da / (da - db);
        return a + (b - a) * t;
    }
    return std::nullopt;
}

bool IntersectionCapCalculator::isPointInPolygon(const IntersectionCapCalculatorVec2& pt, const std::vector<IntersectionCapCalculatorVec2>& poly) {
    int crossings = 0;
    for (size_t i = 0, n = poly.size(); i < n; ++i) {
        const IntersectionCapCalculatorVec2& a = poly[i];
        const IntersectionCapCalculatorVec2& b = poly[(i + 1) % n];
        if (((a.y > pt.y) != (b.y > pt.y)) &&
            (pt.x < (b.x - a.x) * (pt.y - a.y) / (b.y - a.y + 1e-12) + a.x))
            ++crossings;
    }
    return crossings % 2 == 1;
}

bool IntersectionCapCalculator::arePointsEqual(const IntersectionCapCalculatorVec3& a, const IntersectionCapCalculatorVec3& b, double eps) {
    return (std::fabs(a.x - b.x) < eps &&
        std::fabs(a.y - b.y) < eps &&
        std::fabs(a.z - b.z) < eps);
}

// Robust connection of segments to closed loops
std::vector<std::vector<IntersectionCapCalculatorVec3>> IntersectionCapCalculator::extractClosedLoops(const std::vector<IntersectionCapCalculatorSegment>& segments) {
    std::vector<IntersectionCapCalculatorSegment> remaining = segments;
    std::vector<std::vector<IntersectionCapCalculatorVec3>> loops;

    while (!remaining.empty()) {
        std::vector<IntersectionCapCalculatorVec3> loop;
        IntersectionCapCalculatorSegment seg = remaining.back();
        remaining.pop_back();

        loop.push_back(seg.first);
        loop.push_back(seg.second);

        bool extended = true;
        while (extended) {
            extended = false;

            for (auto it = remaining.begin(); it != remaining.end(); ++it) {
                const IntersectionCapCalculatorVec3& a = it->first;
                const IntersectionCapCalculatorVec3& b = it->second;

                if (arePointsEqual(loop.back(), a)) {
                    loop.push_back(b);
                    remaining.erase(it);
                    extended = true;
                    break;
                }
                else if (arePointsEqual(loop.back(), b)) {
                    loop.push_back(a);
                    remaining.erase(it);
                    extended = true;
                    break;
                }
                else if (arePointsEqual(loop.front(), a)) {
                    loop.insert(loop.begin(), b);
                    remaining.erase(it);
                    extended = true;
                    break;
                }
                else if (arePointsEqual(loop.front(), b)) {
                    loop.insert(loop.begin(), a);
                    remaining.erase(it);
                    extended = true;
                    break;
                }
            }
        }

        // Close loop in case it remains open
        if (!arePointsEqual(loop.front(), loop.back())) {
            loop.push_back(loop.front());
        }

        if (loop.size() > 3) {
            loops.push_back(loop);
        }
    }

    return loops;
}

double IntersectionCapCalculator::polygonArea(const std::vector<IntersectionCapCalculatorVec2>& poly) 
{
    double area = 0.0;
    size_t n = poly.size();
    for (size_t i = 0; i < n; ++i) {
        const auto& a = poly[i];
        const auto& b = poly[(i + 1) % n];
        area += (a.x * b.y - b.x * a.y);
    }
    return area * 0.5;
}

void IntersectionCapCalculator::classifyPolygons(const std::vector<std::vector<IntersectionCapCalculatorVec2>>& rings, 
                                                 std::vector<std::vector<IntersectionCapCalculatorVec2>>& out_rings, 
                                                 std::vector<std::vector<IntersectionCapCalculatorVec2>>& holes) 
{
    // Calculate area and sort polygons
    struct RingWithArea {
        std::vector<IntersectionCapCalculatorVec2> ring;
        double absArea;
    };

    std::vector<RingWithArea> sorted;
    for (const auto& r : rings) {
        double area = polygonArea(r);
        sorted.push_back({ r, std::abs(area) });
    }

    std::sort(sorted.begin(), sorted.end(),
        [](const RingWithArea& a, const RingWithArea& b) {
            return a.absArea > b.absArea;
        });

    for (size_t i = 0; i < sorted.size(); ++i) {
        const auto& ring = sorted[i].ring;
        IntersectionCapCalculatorVec2 centroid = { 0, 0 };
        for (const auto& p : ring) {
            centroid.x += p.x;
            centroid.y += p.y;
        }
        centroid.x /= ring.size();
        centroid.y /= ring.size();

        bool inside = false;
		for (size_t j = 0; j < i; ++j) { // Check against larger rings
            if (isPointInPolygon(centroid, sorted[j].ring)) {
                inside = true;
                break;
            }
        }

        (inside ? holes : out_rings).push_back(ring);
    }
}

void IntersectionCapCalculator::determineCutSegments(const IntersectionCapCalculatorPlane& plane, SceneNodeGeometry* geometryItem, std::vector<IntersectionCapCalculatorSegment>& segments)
{
    osg::Transform* triangleTransform = dynamic_cast<osg::Transform*>(geometryItem->getTriangles());
    assert(triangleTransform != nullptr);

    assert(triangleTransform->getNumChildren() == 1);
    osg::Geode* triangleNode = dynamic_cast<osg::Geode*>(triangleTransform->getChild(0));
    assert(triangleNode != nullptr);

    if (triangleNode != nullptr)
    {
        for (int i = 0; i < triangleNode->getNumDrawables(); i++)
        {
            osg::Geometry* geometry = dynamic_cast<osg::Geometry*>(triangleNode->getDrawable(i));

            if (geometry != nullptr)
            {
                osg::Vec3Array* vertices = dynamic_cast<osg::Vec3Array*>(geometry->getVertexArray());

                unsigned int numberOfTriangles = vertices->getNumElements() / 3;

                for (unsigned int index = 0; index < numberOfTriangles; index++) {

                    const auto& p1 = (*vertices)[3 * index + 0];
                    const auto& p2 = (*vertices)[3 * index + 1];
                    const auto& p3 = (*vertices)[3 * index + 2];

                    IntersectionCapCalculatorVec3 v0(p1.x(), p1.y(), p1.z());
                    IntersectionCapCalculatorVec3 v1(p2.x(), p2.y(), p2.z());
                    IntersectionCapCalculatorVec3 v2(p3.x(), p3.y(), p3.z());

                    double d0 = (v0 - plane.point).dot(plane.normal);
                    double d1 = (v1 - plane.point).dot(plane.normal);
                    double d2 = (v2 - plane.point).dot(plane.normal);

                    std::vector<IntersectionCapCalculatorVec3> isects;

                    auto p01 = intersectEdge(v0, v1, d0, d1);
                    auto p12 = intersectEdge(v1, v2, d1, d2);
                    auto p20 = intersectEdge(v2, v0, d2, d0);

                    if (p01) isects.push_back(*p01);
                    if (p12) isects.push_back(*p12);
                    if (p20) isects.push_back(*p20);

                    if (isects.size() == 2) {
                        segments.emplace_back(isects[0], isects[1]);
                    }
                }
            }
        }
    }
}

void IntersectionCapCalculator::buildTriangleVisualizationNode(SceneNodeGeometry* geometryItem, const osg::Vec3d& normal, 
                                                               const std::vector<IntersectionCapCalculatorTriangle3D> &triangles,
                                                               std::vector<IntersectionCapCalculatorVec2> &texcoords_out)
{
    std::string materialType = "Rough";

    ViewerSettings* settings = ViewerSettings::instance();

    double colorRGB[] = { settings->cutplaneFillColor.r() / 255.0,  settings->cutplaneFillColor.g() / 255.0,  settings->cutplaneFillColor.b() / 255.0 };
    double transparency = 0.0;

    bool useShapeColor = settings->cutplaneColorFromObject;

    if (useShapeColor)
    {
        geometryItem->getSurfaceColorRGB(colorRGB);
    }

    osg::ref_ptr<osg::Material> material = new osg::Material;

    SceneNodeMaterial sceneNodeMaterial;
    auto materialSet = sceneNodeMaterial.setMaterial(material, materialType, colorRGB[0], colorRGB[1], colorRGB[2], transparency);

    // Allocate and dimensions arrays for nodes and normals (each triangle has three nodes - the nodes are not shared between adjacent triangles)
    unsigned long long nTriangles = triangles.size();
    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(nTriangles * 3);
    osg::ref_ptr<osg::Vec2Array> texcoords = new osg::Vec2Array(nTriangles * 3);
    osg::ref_ptr<osg::Vec3Array> normals = new osg::Vec3Array(nTriangles * 3);

    // Now store the triangle vertices in the nodes and normals array

    unsigned long long nVertex = 0;
    unsigned long long nNormal = 0;
    unsigned long long nTexture = 0;

    for (auto triangle : triangles)
    {
        vertices->at(nVertex).set(triangle.a.x, triangle.a.y, triangle.a.z);
        vertices->at(nVertex + 1).set(triangle.b.x, triangle.b.y, triangle.b.z);
        vertices->at(nVertex + 2).set(triangle.c.x, triangle.c.y, triangle.c.z);

        nVertex += 3;

        normals->at(nNormal).set(normal.x(), normal.y(), normal.z());
        normals->at(nNormal + 1).set(normal.x(), normal.y(), normal.z());
        normals->at(nNormal + 2).set(normal.x(), normal.y(), normal.z());

        nNormal += 3;

        texcoords->at(nTexture).set(texcoords_out[nTexture].x, texcoords_out[nTexture].y);
        texcoords->at(nTexture + 1).set(texcoords_out[nTexture + 1].x, texcoords_out[nTexture + 1].y);
        texcoords->at(nTexture + 2).set(texcoords_out[nTexture + 2].x, texcoords_out[nTexture + 2].y);

        nTexture += 3;
    }

    // Store the color in a color array (the color will be shared among all nodes, so only one entry is needed)
    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
    colors->push_back(osg::Vec4(colorRGB[0], colorRGB[1], colorRGB[2], transparency));

    // Create the geometry object to store the data
    osg::ref_ptr<osg::Geometry> newGeometry = new osg::Geometry;

    if (!ViewerSettings::instance()->useDisplayLists)
    {
        newGeometry->setUseDisplayList(false);
        newGeometry->setUseVertexBufferObjects(ViewerSettings::instance()->useVertexBufferObjects);
    }

    newGeometry->getOrCreateStateSet()->setMode(GL_CULL_FACE, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);

    osg::LightModel* p = new osg::LightModel;
    p->setTwoSided(true);
    newGeometry->getOrCreateStateSet()->setAttributeAndModes(p);
    newGeometry->getOrCreateStateSet()->setAttributeAndModes(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::FILL));

    newGeometry->setVertexArray(vertices);

    newGeometry->setNormalArray(normals);
    newGeometry->setNormalBinding(osg::Geometry::BIND_PER_VERTEX);

    newGeometry->setTexCoordArray(0, texcoords);

    newGeometry->setColorArray(colors);
    newGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

    newGeometry->addPrimitiveSet(new osg::DrawArrays(GL_TRIANGLES, 0, nTriangles * 3));

    newGeometry->getOrCreateStateSet()->setAttribute(material);
    newGeometry->getOrCreateStateSet()->setAttributeAndModes(new osg::PolygonOffset(2.0f, 2.0f));

    if (settings->cutplaneTexture)
    {
        osg::ref_ptr<osg::Texture2D> texture = TextureMapManager::getTexture("Stripes");

        texture->setWrap(osg::Texture::WRAP_S, osg::Texture::REPEAT);
        texture->setWrap(osg::Texture::WRAP_T, osg::Texture::REPEAT);

        newGeometry->getOrCreateStateSet()->setTextureAttributeAndModes(0, texture, osg::StateAttribute::ON);
    }

    delete materialSet;
    materialSet = nullptr;

    geometryItem->setCutCapGeometryTriangles(newGeometry);
}

void IntersectionCapCalculator::buildEdgeVisualizationNode(SceneNodeGeometry* geometryItem, std::vector<std::vector<IntersectionCapCalculatorVec3>>& loops)
{
    ViewerSettings* settings = ViewerSettings::instance();

    double colorR    = settings->cutplaneOutlineColor.r() / 255.0;
    double colorG    = settings->cutplaneOutlineColor.g() / 255.0;
    double colorB    = settings->cutplaneOutlineColor.b() / 255.0;
    double lineWidth = settings->cutplaneOutlineWidth;

    osg::ref_ptr<osg::Vec4Array> colors = new osg::Vec4Array;
    colors->push_back(osg::Vec4(colorR, colorG, colorB, 0.0));

    // Create the geometry object to store the data
    osg::ref_ptr<osg::Geometry> newGeometry = new osg::Geometry;

    if (!ViewerSettings::instance()->useDisplayLists)
    {
        newGeometry->setUseDisplayList(false);
        newGeometry->setUseVertexBufferObjects(ViewerSettings::instance()->useVertexBufferObjects);
    }

    // Count edges
    int nEdges = 0;
    for (auto loop : loops)
    {
        nEdges += (loop.size() - 1);
    }

    osg::ref_ptr<osg::Vec3Array> vertices = new osg::Vec3Array(nEdges * 2);

    // Now store the edge vertices in the array

    unsigned long long nVertex = 0;

    for (auto loop : loops)
    {
        unsigned long long loopVertex = 0;

        for (auto point : loop)
        {
            vertices->at(nVertex).set(point.x, point.y, point.z);
            nVertex++;

            if (loopVertex != 0 && loopVertex != loop.size()-1)
            {
                vertices->at(nVertex).set(point.x, point.y, point.z);
                nVertex++;
            }

            loopVertex++;
        }
    }

    newGeometry->setVertexArray(vertices);

    newGeometry->setColorArray(colors);
    newGeometry->setColorBinding(osg::Geometry::BIND_OVERALL);

    newGeometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, nEdges * 2));

    osg::StateSet* ss = newGeometry->getOrCreateStateSet();

    ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    ss->setMode(GL_BLEND, osg::StateAttribute::OFF);
    ss->setAttributeAndModes(new osg::PolygonMode(osg::PolygonMode::FRONT_AND_BACK, osg::PolygonMode::LINE));
    ss->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
    ss->setAttribute(new osg::LineWidth(lineWidth), osg::StateAttribute::ON);

    geometryItem->setCutCapGeometryEdges(newGeometry);
}

void IntersectionCapCalculator::buildProjectionBasis(const osg::Vec3d& normal, const osg::Vec3d& origin, osg::Vec3d &u, osg::Vec3d &v)
{
    osg::Vec3d up = std::abs(normal.z()) < 0.99 ? osg::Vec3d(0.0, 0.0, 1.0) : osg::Vec3d(0.0, 1.0, 0.0);
    u = normal ^ up;  // Cross product: normal x up -> Basis-U
    u.normalize();
    v = normal ^ u;  // Basis-V (normal x u)
    v.normalize();
}

void IntersectionCapCalculator::generateCapGeometryAndVisualization(SceneNodeGeometry* geometryItem, const osg::Vec3d& normal, const osg::Vec3d& point, double radius)
{
    geometryItem->deleteCutCapGeometryTriangles();
    geometryItem->deleteCutCapGeometryEdges();

    osg::Vec3d movedPoint = point + normal * radius * 1e-4;

	IntersectionCapCalculatorPlane plane = { {movedPoint.x(), movedPoint.y(), movedPoint.z()}, {normal.x(), normal.y(), normal.z()}};

	std::vector<IntersectionCapCalculatorSegment> segments;

    determineCutSegments(plane, geometryItem, segments);

    auto loops3D = extractClosedLoops(segments);

    buildEdgeVisualizationNode(geometryItem, loops3D);

    ViewerSettings* settings = ViewerSettings::instance();

    if (settings->cutplaneDrawSolid)
    {
        IntersectionCapCalculatorProjection2D proj = makeProjectionBasis(plane);

        std::vector<std::vector<IntersectionCapCalculatorVec2>> all_rings_2D;
        for (auto& loop3D : loops3D) {
            std::vector<IntersectionCapCalculatorVec2> ring2D;
            for (auto& p : loop3D)
                ring2D.push_back(proj.project(p));
            all_rings_2D.push_back(ring2D);
        }

        std::vector<std::vector<IntersectionCapCalculatorVec2>> outer_rings, hole_rings;
        classifyPolygons(all_rings_2D, outer_rings, hole_rings);

        using Polygon = std::vector<std::vector<IntersectionCapCalculatorVec2>>;
        std::vector<IntersectionCapCalculatorTriangle3D> triangles_out;
        std::vector<IntersectionCapCalculatorVec2> texcoords_out;

        osg::Vec3d u, v;
        buildProjectionBasis(normal, point, u, v);

        IntersectionCapCalculatorVec3 uDir(u.x(), u.y(), u.z());
        IntersectionCapCalculatorVec3 vDir(v.x(), v.y(), v.z());
        IntersectionCapCalculatorVec3 origin(point.x(), point.y(), point.z());

        float repeatPerMeter = (float)(1.5 / (radius > 0.0 ? radius : 1.0));

        for (auto& outer : outer_rings) {
            Polygon poly = { outer };
            for (auto& hole : hole_rings) {
                if (isPointInPolygon(hole[0], outer)) poly.push_back(hole);
            }

            auto indices = mapbox::earcut<uint32_t>(poly);

            std::vector<IntersectionCapCalculatorVec2> flat_points;
            for (auto& ring : poly)
                for (auto& p : ring)
                    flat_points.push_back(p);

            for (size_t i = 0; i < indices.size(); i += 3) {
                IntersectionCapCalculatorVec2 p0_2D = flat_points[indices[i]];
                IntersectionCapCalculatorVec2 p1_2D = flat_points[indices[i + 1]];
                IntersectionCapCalculatorVec2 p2_2D = flat_points[indices[i + 2]];

                // Backprojection in 3D
                IntersectionCapCalculatorVec3 v0 = proj.unproject(p0_2D);
                IntersectionCapCalculatorVec3 v1 = proj.unproject(p1_2D);
                IntersectionCapCalculatorVec3 v2 = proj.unproject(p2_2D);

                for (auto& p3D : { v0, v1, v2 }) {
                    IntersectionCapCalculatorVec3 rel = p3D - origin;
                    float u = dot(rel, uDir) * repeatPerMeter;
                    float v = dot(rel, vDir) * repeatPerMeter;
                    texcoords_out.push_back({ u, v });
                }

                // Store the triangles
                triangles_out.push_back({ v0, v1, v2 });
            }
        }

        buildTriangleVisualizationNode(geometryItem, normal, triangles_out, texcoords_out);
    }
}


