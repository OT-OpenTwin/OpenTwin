// @otlicense
// File: EntityCableHarness.cpp
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

// OpenTwin header
#include "OTCADEntities/EntityCableHarness.h"
#include "OTModelEntities/EntityBinaryData.h"

#include "OTCADEntities/tinyxml2.h"

static EntityFactoryRegistrar<EntityCableHarness> registrar(EntityCableHarness::className());

EntityCableHarness::EntityCableHarness(ot::UID ID, EntityBase* parent, EntityObserver* obs, ModelState* ms) :
	EntitySmartPart(ID, parent, obs, ms)
{

}

EntityCableHarness::~EntityCableHarness()
{

}

void EntityCableHarness::updatePart(void)
{
	// Read the xml data from the data entity
	std::string harnessData(getData()->getData().data());

	// Now we parse the harness xml file, extract nodes and segments and finally create the harness geometry
	std::map<std::string, std::tuple<double, double, double>> harnessNodes;
	std::set<std::tuple<std::string, std::string>> harnessSegments;

	extractHarnessData(harnessData, harnessNodes, harnessSegments);

	int numberSegments = 10;
	double tubeRadius = 0.0;

	EntityPropertiesDouble* cableRadiusProperty = dynamic_cast<EntityPropertiesDouble*>(getProperties().getProperty("#Cable radius"));
	if (cableRadiusProperty != nullptr)
	{
		tubeRadius = cableRadiusProperty->getValue();
	}

	createHarnessFacets(harnessNodes, harnessSegments, getFacets()->getNodeVector(), getFacets()->getTriangleList(), getFacets()->getEdgeList(), tubeRadius, numberSegments);
}

void EntityCableHarness::extractHarnessData(const std::string& harnessData,
											std::map<std::string, std::tuple<double, double, double>>& harnessNodes,
											std::set<std::tuple<std::string, std::string>>& harnessSegments)
{
	tinyxml2::XMLDocument doc;
	doc.Parse(harnessData.c_str(), harnessData.size());

	tinyxml2::XMLElement* root = doc.FirstChildElement("Harness");
	if (!root) return;

	tinyxml2::XMLElement* knots = root->FirstChildElement("Knots");
	if (!knots) return;

	for (tinyxml2::XMLElement* knot = knots->FirstChildElement("Knot");
		knot != nullptr;
		knot = knot->NextSiblingElement("Knot")) {

		const char* id = knot->Attribute("ID");

		double x = 0, y = 0, z = 0;
		knot->QueryDoubleAttribute("X", &x);
		knot->QueryDoubleAttribute("Y", &y);
		knot->QueryDoubleAttribute("Z", &z);

		harnessNodes[id] = std::tuple<double, double, double>(x, y, z);
	}

	tinyxml2::XMLElement* segments = root->FirstChildElement("Segments");
	if (!segments) return;

	for (tinyxml2::XMLElement* segment = segments->FirstChildElement("Segment");
		segment != nullptr;
		segment = segment->NextSiblingElement("Segment")) {

		const char* knot1 = segment->Attribute("Knot1ID");
		const char* knot2 = segment->Attribute("Knot2ID");

		std::string knot1ID = knot1 ? knot1 : "";
		std::string knot2ID = knot2 ? knot2 : "";

		harnessSegments.emplace(std::tuple <std::string, std::string>(knot1ID, knot2ID));
	}
}

void EntityCableHarness::createHarnessFacets(std::map<std::string, std::tuple<double, double, double>>& harnessNodes,
											 std::set<std::tuple<std::string, std::string>>& harnessSegments,
											 std::vector<Geometry::Node>& nodes,
											 std::list<Geometry::Triangle>& triangles,
											 std::list<Geometry::Edge>& edges,
											 double tubeRadius,
											 int nTubeSegments)
{
	// Here we process segment by segment and create each of the segments as a little tube with a given number of segments. 
	double totalSegmentLength = 0.0;

	for (auto segment : harnessSegments)
	{
		std::tuple<double, double, double> startPoint = harnessNodes[std::get<0>(segment)];
		std::tuple<double, double, double> endPoint = harnessNodes[std::get<1>(segment)];

		double p1[] = { std::get<0>(startPoint), std::get<1>(startPoint), std::get<2>(startPoint) };
		double p2[] = { std::get<0>(endPoint), std::get<1>(endPoint), std::get<2>(endPoint) };

		double segmentLength = sqrt((p1[0] - p2[0]) * (p1[0] - p2[0]) + (p1[1] - p2[1]) * (p1[1] - p2[1]) + (p1[2] - p2[2]) * (p1[2] - p2[2]));

		totalSegmentLength += segmentLength;
	}

	std::list<Geometry::Node> nodesList;

	if (tubeRadius == 0.0)
	{
		tubeRadius = 0.01 * totalSegmentLength;
	}
	int faceId = 1;

	for (auto segment : harnessSegments)
	{
		std::tuple<double, double, double> startPoint = harnessNodes[std::get<0>(segment)];
		std::tuple<double, double, double> endPoint = harnessNodes[std::get<1>(segment)];

		double p1[] = { std::get<0>(startPoint), std::get<1>(startPoint), std::get<2>(startPoint) };
		double p2[] = { std::get<0>(endPoint), std::get<1>(endPoint), std::get<2>(endPoint) };

		addCapsule(p1, p2, tubeRadius, nTubeSegments, faceId, nodesList, triangles);
	}

	// Finally, we store all nodes from the nodes list in the nodes vector
	nodes.reserve(nodesList.size());
	for (const auto& n : nodesList)
	{
		nodes.push_back(n);
	}
}

void EntityCableHarness::addCapsule(const double p1[3], const double p2[3],
									double tubeRadius, int nTubeSegments, ot::UID faceId,
									std::list<Geometry::Node>& nodes, std::list<Geometry::Triangle>& triangles)
{
	// Minimal 3 segments for a valid tube, hemisphere resolution derived from it
	const int segs = std::max(3, nTubeSegments);
	const int hemiRings = std::max(2, segs / 2);

	// Simple internal vector helper
	struct V3 {
		double x, y, z;
		V3 operator + (const V3& o) const { return { x + o.x, y + o.y, z + o.z }; }
		V3 operator - (const V3& o) const { return { x - o.x, y - o.y, z - o.z }; }
		V3 operator * (double s)   const { return { x * s, y * s, z * s }; }
	};

	// Basic vector math
	auto dot = [](const V3& a, const V3& b) { return a.x * b.x + a.y * b.y + a.z * b.z; };
	auto cross = [](const V3& a, const V3& b) { return V3{ a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x }; };
	auto len = [&](const V3& v) { return std::sqrt(dot(v, v)); };
	auto norm = [&](const V3& v) { double l = len(v); return l > 1e-20 ? v * (1.0 / l) : V3{ 0.0,0.0,1.0 }; };

	V3 P1{ p1[0], p1[1], p1[2] };
	V3 P2{ p2[0], p2[1], p2[2] };

	// Capsule axis and length
	V3 axis = P2 - P1;
	double h = len(axis);
	if (h <= 1e-20 || tubeRadius <= 0.0) return;

	// Build orthonormal basis (u, v, w)
	// w = axis direction, u/v span the circular cross section
	V3 w = axis * (1.0 / h);
	V3 tmp = (std::fabs(w.z) < 0.999) ? V3{ 0.0,0.0,1.0 } : V3{ 0.0,1.0,0.0 };
	V3 u = norm(cross(tmp, w));
	V3 v = cross(w, u);

	// Append a node and return its index
	auto addNode = [&](const V3& p, const V3& n, double uu, double vv) -> ot::UID {
		Geometry::Node nd;
		nd.setCoords(p.x, p.y, p.z);
		nd.setNormals(n.x, n.y, n.z);
		nd.setUVpar(uu, vv);
		nodes.push_back(nd);
		return (ot::UID)(nodes.size() - 1);
		};

	// Append a triangle
	auto addTri = [&](ot::UID a, ot::UID b, ot::UID c) {
		triangles.emplace_back(a, b, c, faceId);
		};

	// Create one ring of vertices around the axis
	// center = sphere center or cylinder endpoint
	// ringR  = radius of the ring
	// zOff   = offset along axis from center
	auto addRing = [&](const V3& center, double ringR, double zOff, bool /*bottomCap*/, double vTex) {
		std::vector<ot::UID> ring;
		ring.reserve(segs);

		for (int i = 0; i < segs; ++i) {
			double ang = 2.0 * M_PI * double(i) / double(segs);
			double ca = std::cos(ang), sa = std::sin(ang);

			// Radial direction in cross-section plane
			V3 radial = u * ca + v * sa;

			// Final vertex position
			V3 pos = center + radial * ringR + w * zOff;

			// Compute normal:
			// - cylinder: purely radial
			// - hemispheres: from sphere center
			V3 nrm = norm(radial * ringR + w * zOff);
			if (ringR == tubeRadius && zOff == 0.0)
				nrm = radial;

			ring.push_back(addNode(pos, nrm, double(i) / double(segs), vTex));
		}
		return ring;
		};

	std::vector<std::vector<ot::UID>> rings;
	rings.reserve(2 * hemiRings + 2);

	// --- Bottom hemisphere (center = P1) ---
	// from bottom pole (-90°) to equator (0°)
	for (int j = 0; j <= hemiRings; ++j) {
		double t = double(j) / double(hemiRings);
		double phi = -0.5 * M_PI + t * 0.5 * M_PI;
		double rr = tubeRadius * std::cos(phi);
		double zz = tubeRadius * std::sin(phi);
		double vv = t * 0.5;

		rings.push_back(addRing(P1, rr, zz, true, vv));
	}

	// --- Cylinder top ring (center = P2) ---
	rings.push_back(addRing(P2, tubeRadius, 0.0, false, 0.5));

	// --- Top hemisphere (center = P2) ---
	// from equator (0°) to top pole (+90°)
	for (int j = 1; j <= hemiRings; ++j) {
		double t = double(j) / double(hemiRings);
		double phi = t * 0.5 * M_PI;
		double rr = tubeRadius * std::cos(phi);
		double zz = tubeRadius * std::sin(phi);
		double vv = 0.5 + t * 0.5;

		rings.push_back(addRing(P2, rr, zz, false, vv));
	}

	// --- Connect rings with triangles ---
	for (size_t r = 0; r + 1 < rings.size(); ++r) {
		for (int i = 0; i < segs; ++i) {
			ot::UID a = rings[r][i];
			ot::UID b = rings[r][(i + 1) % segs];
			ot::UID c = rings[r + 1][i];
			ot::UID d = rings[r + 1][(i + 1) % segs];

			addTri(a, b, c);
			addTri(c, b, d);
		}
	}
}

void EntityCableHarness::createPartProperties(void)
{
	addPartProperty("Cable radius", "0");
}

