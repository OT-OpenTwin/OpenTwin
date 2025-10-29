// @otlicense

#pragma once

#include <optional>

#include <osg/Vec3d>

class SceneNodeGeometry;

struct IntersectionCapCalculatorVec2 {
    double x, y;
};

struct IntersectionCapCalculatorVec3 {
    double x, y, z;

    IntersectionCapCalculatorVec3(double _x, double _y, double _z) { x = _x; y = _y, z = _z; }

    IntersectionCapCalculatorVec3 operator-(const IntersectionCapCalculatorVec3& o) const { return { x - o.x, y - o.y, z - o.z }; }
    IntersectionCapCalculatorVec3 operator+(const IntersectionCapCalculatorVec3& o) const { return { x + o.x, y + o.y, z + o.z }; }
    IntersectionCapCalculatorVec3 operator*(double s) const { return { x * s, y * s, z * s }; }

    double dot(const IntersectionCapCalculatorVec3& o) const { return x * o.x + y * o.y + z * o.z; }
    IntersectionCapCalculatorVec3 cross(const IntersectionCapCalculatorVec3& o) const {
        return {
            y * o.z - z * o.y,
            z * o.x - x * o.z,
            x * o.y - y * o.x
        };
    }
    double norm() const { return sqrt(x * x + y * y + z * z); }
    IntersectionCapCalculatorVec3 normalized() const { double n = norm(); return { x / n, y / n, z / n }; }

    bool operator==(const IntersectionCapCalculatorVec3& o) const {
        const double eps = 1e-6;
        return fabs(x - o.x) < eps && fabs(y - o.y) < eps && fabs(z - o.z) < eps;
    }
    bool operator<(const IntersectionCapCalculatorVec3& o) const {
        if (fabs(x - o.x) > 1e-6) return x < o.x;
        if (fabs(y - o.y) > 1e-6) return y < o.y;
        return z < o.z;
    }
};

struct IntersectionCapCalculatorPlane {
    IntersectionCapCalculatorVec3 point;
    IntersectionCapCalculatorVec3 normal;
};

struct IntersectionCapCalculatorTriangle3D {
    IntersectionCapCalculatorVec3 a, b, c;
};

struct IntersectionCapCalculatorProjection2D {
    IntersectionCapCalculatorVec3 origin, u, v;
    IntersectionCapCalculatorVec2 project(const IntersectionCapCalculatorVec3& p) const {
        IntersectionCapCalculatorVec3 d = p - origin;
        return { d.dot(u), d.dot(v) };
    }
    IntersectionCapCalculatorVec3 unproject(const IntersectionCapCalculatorVec2& p) const {
        return origin + u * p.x + v * p.y;
    }
};

typedef  std::pair<IntersectionCapCalculatorVec3, IntersectionCapCalculatorVec3> IntersectionCapCalculatorSegment;

class IntersectionCapCalculator {
public:
	IntersectionCapCalculator();
	virtual ~IntersectionCapCalculator();

    void generateCapGeometryAndVisualization(SceneNodeGeometry* geometryItem, const osg::Vec3d &normal, const osg::Vec3d &point, double radius);

private:
    void determineCutSegments(const IntersectionCapCalculatorPlane& plane, SceneNodeGeometry* geometryItem, std::vector<IntersectionCapCalculatorSegment>& segments);
    std::optional<IntersectionCapCalculatorVec3> intersectEdge(const IntersectionCapCalculatorVec3& a, const IntersectionCapCalculatorVec3& b, double da, double db);
    bool isPointInPolygon(const IntersectionCapCalculatorVec2 & pt, const std::vector<IntersectionCapCalculatorVec2>&poly);
    bool arePointsEqual(const IntersectionCapCalculatorVec3& a, const IntersectionCapCalculatorVec3& b, double eps = 1e-6);
    std::vector<std::vector<IntersectionCapCalculatorVec3>> extractClosedLoops(const std::vector<IntersectionCapCalculatorSegment>& segments);
    double polygonArea(const std::vector<IntersectionCapCalculatorVec2>& poly);
    void classifyPolygons(const std::vector<std::vector<IntersectionCapCalculatorVec2>>& rings, std::vector<std::vector<IntersectionCapCalculatorVec2>>& out_rings, std::vector<std::vector<IntersectionCapCalculatorVec2>>& holes);
    IntersectionCapCalculatorProjection2D makeProjectionBasis(const IntersectionCapCalculatorPlane& plane);
    void buildTriangleVisualizationNode(SceneNodeGeometry* geometryItem, const osg::Vec3d& normal, const std::vector<IntersectionCapCalculatorTriangle3D>& triangles, std::vector<IntersectionCapCalculatorVec2> &texcoords_out);
    void buildEdgeVisualizationNode(SceneNodeGeometry* geometryItem, std::vector<std::vector<IntersectionCapCalculatorVec3>> &loops);
    void buildProjectionBasis(const osg::Vec3d& normal, const osg::Vec3d& origin, osg::Vec3d& u, osg::Vec3d& v);
    double dot(const IntersectionCapCalculatorVec3& a, const IntersectionCapCalculatorVec3& b) {return a.x * b.x + a.y * b.y + a.z * b.z;}
};
