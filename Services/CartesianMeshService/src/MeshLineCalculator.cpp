// @otlicense
// File: MeshLineCalculator.h
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

#include "MeshLineCalculator.h"
#include "CartesianMeshMaterial.h"

#include "OTCADEntities/EntityGeometry.h"

void MeshLineCalculator::updateMeshLines()
{
	// This is the general controller for the mesh line calculation
	BoundingBox geometryBoundingBox = calculateBoundingBox();
	BoundingBox meshBoundingBox = determineBoundingBoxExtension(geometryBoundingBox);

	// Find the base step width
	double baseStepWidth = std::min(maximumEdgeLength, geometryBoundingBox.getDiagonal() / stepsAlongDiagonal);
	double geometryToleranceAbsolute = geometryTolerance * geometryBoundingBox.getDiagonal();
	double smallestMeshStep = smallestCellRatio * baseStepWidth;

	determineMeshLinesOneDirection(meshBoundingBox.getXmin(), meshBoundingBox.getXmax(), baseStepWidth, meshCoords[0]);
	determineMeshLinesOneDirection(meshBoundingBox.getYmin(), meshBoundingBox.getYmax(), baseStepWidth, meshCoords[1]);
	determineMeshLinesOneDirection(meshBoundingBox.getZmin(), meshBoundingBox.getZmax(), baseStepWidth, meshCoords[2]);

	std::list<MeshLineCalculatorWeightedPoint> fixPlanesX = determineFixPlanes(0, 1.0, 0.0, 0.0);
	std::list<MeshLineCalculatorWeightedPoint> fixPlanesY = determineFixPlanes(1, 0.0, 1.0, 0.0);
	std::list<MeshLineCalculatorWeightedPoint> fixPlanesZ = determineFixPlanes(2, 0.0, 0.0, 1.0);

	// The next step merges triangles which are close within the geometric tolerance and adds their areas. This is relevant for the 
	// case that a planar face is built by multiple triangles. Afterward, the face will have one location together with its total area.
	std::list<MeshLineCalculatorWeightedPoint> mergedFixPlanesX = mergeValuesAndAveragePositions(fixPlanesX, geometryToleranceAbsolute);
	std::list<MeshLineCalculatorWeightedPoint> mergedFixPlanesY = mergeValuesAndAveragePositions(fixPlanesY, geometryToleranceAbsolute);
	std::list<MeshLineCalculatorWeightedPoint> mergedFixPlanesZ = mergeValuesAndAveragePositions(fixPlanesZ, geometryToleranceAbsolute);

	// Now we merge all fix planes according to the smalles mesh step size (everything close than this is merged). This considers the 
	// object priority as well.
	std::list<MeshLineCalculatorWeightedPoint> finalFixPlanesX = mergeWeightedPoints(mergedFixPlanesX, smallestMeshStep);
	std::list<MeshLineCalculatorWeightedPoint> finalFixPlanesY = mergeWeightedPoints(mergedFixPlanesY, smallestMeshStep);
	std::list<MeshLineCalculatorWeightedPoint> finalFixPlanesZ = mergeWeightedPoints(mergedFixPlanesZ, smallestMeshStep);

	// Next, we determine the stepwidth profile from the objects bounding boxes
	std::vector<MeshLineCalculatorStepRange> densityRangesX = determineDensityRanges(0, meshBoundingBox.getXmin(), meshBoundingBox.getXmax(), baseStepWidth);
	std::vector<MeshLineCalculatorStepRange> densityRangesY = determineDensityRanges(1, meshBoundingBox.getYmin(), meshBoundingBox.getYmax(), baseStepWidth);
	std::vector<MeshLineCalculatorStepRange> densityRangesZ = determineDensityRanges(2, meshBoundingBox.getZmin(), meshBoundingBox.getZmax(), baseStepWidth);



	// Naechste Schritte:
	// 2. Grobe Gitterlinienarrays bestimmen: bounding box hinzufügen und dann fix planes
	// 4. Aus Density und grobem Gitterlinienarray gemeinsam ein eigentliches Gitterlinienarray bestimmen.
	// 5. Zusätzliche Linien einfügen, um die Anforderungen an die Glattheit des Gitters zu erfüllen
	// 6. Setzen der Ergebnisse in meshCoords, loeschen der Funktion determineMeshLinesOneDirection
	// 7. Parameter der Gittergenerierung einstellbar machen.
	// 8. Anzeige der Gitterlinien in der Schnittebene

}

BoundingBox MeshLineCalculator::calculateBoundingBox()
{
	// Determine the bounding box for all entities
	BoundingBox geometryBoundingBox;

	for (auto entity : meshEntities)
	{
		// Ignore the background shape which does not have a name assigned yet
		if (!entity->getName().empty())
		{
			if (entity->getClassName() != "EntityContainer")
			{
				double xmin = 0.0, xmax = 0.0, ymin = 0.0, ymax = 0.0, zmin = 0.0, zmax = 0.0;

				if (entity->getEntityBox(xmin, xmax, ymin, ymax, zmin, zmax))
				{
					geometryBoundingBox.extend(xmin, xmax, ymin, ymax, zmin, zmax);
				}
			}
		}
	}

	return geometryBoundingBox;
}

BoundingBox MeshLineCalculator::determineBoundingBoxExtension(const BoundingBox &geometryBoundingBox)
{
	// Now extend the bounding box by the settings if needed
	double offsetXmin = 0.0, offsetXmax = 0.0, offsetYmin = 0.0, offsetYmax = 0.0, offsetZmin = 0.0, offsetZmax = 0.0;

	double deltaX = geometryBoundingBox.getXmax() - geometryBoundingBox.getXmin();
	double deltaY = geometryBoundingBox.getYmax() - geometryBoundingBox.getYmin();
	double deltaZ = geometryBoundingBox.getZmax() - geometryBoundingBox.getZmin();

	EntityPropertiesSelection* backgroundMode = dynamic_cast<EntityPropertiesSelection*>(getEntityMesh()->getProperties().getProperty("Background mode"));
	EntityPropertiesBoolean* extendFlag = dynamic_cast<EntityPropertiesBoolean*>(getEntityMesh()->getProperties().getProperty("Extend in all directions"));
	EntityPropertiesEntityList* backgroundMaterial = dynamic_cast<EntityPropertiesEntityList*>(getEntityMesh()->getProperties().getProperty("Background material"));
	EntityPropertiesDouble* boundaryDistanceAllAbs = dynamic_cast<EntityPropertiesDouble*>(getEntityMesh()->getProperties().getProperty("Boundary distance (abs)"));
	EntityPropertiesDouble* boundaryDistanceAllRel = dynamic_cast<EntityPropertiesDouble*>(getEntityMesh()->getProperties().getProperty("Boundary distance (rel)"));
	EntityPropertiesDouble* boundaryDistanceAbsXmin = dynamic_cast<EntityPropertiesDouble*>(getEntityMesh()->getProperties().getProperty("Boundary distance at xmin (abs)"));
	EntityPropertiesDouble* boundaryDistanceAbsXmax = dynamic_cast<EntityPropertiesDouble*>(getEntityMesh()->getProperties().getProperty("Boundary distance at xmax (abs)"));
	EntityPropertiesDouble* boundaryDistanceAbsYmin = dynamic_cast<EntityPropertiesDouble*>(getEntityMesh()->getProperties().getProperty("Boundary distance at ymin (abs)"));
	EntityPropertiesDouble* boundaryDistanceAbsYmax = dynamic_cast<EntityPropertiesDouble*>(getEntityMesh()->getProperties().getProperty("Boundary distance at ymax (abs)"));
	EntityPropertiesDouble* boundaryDistanceAbsZmin = dynamic_cast<EntityPropertiesDouble*>(getEntityMesh()->getProperties().getProperty("Boundary distance at zmin (abs)"));
	EntityPropertiesDouble* boundaryDistanceAbsZmax = dynamic_cast<EntityPropertiesDouble*>(getEntityMesh()->getProperties().getProperty("Boundary distance at zmax (abs)"));
	EntityPropertiesDouble* boundaryDistanceRelXmin = dynamic_cast<EntityPropertiesDouble*>(getEntityMesh()->getProperties().getProperty("Boundary distance at xmin (rel)"));
	EntityPropertiesDouble* boundaryDistanceRelXmax = dynamic_cast<EntityPropertiesDouble*>(getEntityMesh()->getProperties().getProperty("Boundary distance at xmax (rel)"));
	EntityPropertiesDouble* boundaryDistanceRelYmin = dynamic_cast<EntityPropertiesDouble*>(getEntityMesh()->getProperties().getProperty("Boundary distance at ymin (rel)"));
	EntityPropertiesDouble* boundaryDistanceRelYmax = dynamic_cast<EntityPropertiesDouble*>(getEntityMesh()->getProperties().getProperty("Boundary distance at ymax (rel)"));
	EntityPropertiesDouble* boundaryDistanceRelZmin = dynamic_cast<EntityPropertiesDouble*>(getEntityMesh()->getProperties().getProperty("Boundary distance at zmin (rel)"));
	EntityPropertiesDouble* boundaryDistanceRelZmax = dynamic_cast<EntityPropertiesDouble*>(getEntityMesh()->getProperties().getProperty("Boundary distance at zmax (rel)"));

	if (backgroundMode != nullptr && extendFlag != nullptr && backgroundMaterial != nullptr && boundaryDistanceAllAbs != nullptr && boundaryDistanceAllRel != nullptr
		&& boundaryDistanceAbsXmin != nullptr && boundaryDistanceAbsXmax != nullptr && boundaryDistanceAbsYmin != nullptr && boundaryDistanceAbsYmax != nullptr
		&& boundaryDistanceAbsZmin != nullptr && boundaryDistanceAbsZmax != nullptr
		&& boundaryDistanceRelXmin != nullptr && boundaryDistanceRelXmax != nullptr && boundaryDistanceRelYmin != nullptr && boundaryDistanceRelYmax != nullptr
		&& boundaryDistanceRelZmin != nullptr && boundaryDistanceRelZmax != nullptr)
	{
		if (backgroundMode->getValue() == "Extend relative")
		{
			if (extendFlag->getValue())
			{
				// Same relative extension in all directions
				offsetXmin = boundaryDistanceAllRel->getValue() * deltaX;
				offsetXmax = boundaryDistanceAllRel->getValue() * deltaX;
				offsetYmin = boundaryDistanceAllRel->getValue() * deltaY;
				offsetYmax = boundaryDistanceAllRel->getValue() * deltaY;
				offsetZmin = boundaryDistanceAllRel->getValue() * deltaZ;
				offsetZmax = boundaryDistanceAllRel->getValue() * deltaZ;
			}
			else
			{
				// Direction specific relative extension
				offsetXmin = boundaryDistanceRelXmin->getValue() * deltaX;
				offsetXmax = boundaryDistanceRelXmax->getValue() * deltaX;
				offsetYmin = boundaryDistanceRelYmin->getValue() * deltaY;
				offsetYmax = boundaryDistanceRelYmax->getValue() * deltaY;
				offsetZmin = boundaryDistanceRelZmin->getValue() * deltaZ;
				offsetZmax = boundaryDistanceRelZmax->getValue() * deltaZ;
			}
		}
		else if (backgroundMode->getValue() == "Extend absolute")
		{
			if (extendFlag->getValue())
			{
				// Same absolute extension in all directions
				offsetXmin = boundaryDistanceAllAbs->getValue();
				offsetXmax = boundaryDistanceAllAbs->getValue();
				offsetYmin = boundaryDistanceAllAbs->getValue();
				offsetYmax = boundaryDistanceAllAbs->getValue();
				offsetZmin = boundaryDistanceAllAbs->getValue();
				offsetZmax = boundaryDistanceAllAbs->getValue();
			}
			else
			{
				// Direction specific absolute extension
				offsetXmin = boundaryDistanceAbsXmin->getValue();
				offsetXmax = boundaryDistanceAbsXmax->getValue();
				offsetYmin = boundaryDistanceAbsYmin->getValue();
				offsetYmax = boundaryDistanceAbsYmax->getValue();
				offsetZmin = boundaryDistanceAbsZmin->getValue();
				offsetZmax = boundaryDistanceAbsZmax->getValue();
			}
		}
	}

	BoundingBox meshBoundingBox = geometryBoundingBox;
	meshBoundingBox.extend(geometryBoundingBox.getXmin() - offsetXmin, geometryBoundingBox.getXmax() + offsetXmax, 
						   geometryBoundingBox.getYmin() - offsetYmin, geometryBoundingBox.getYmax() + offsetYmax, 
						   geometryBoundingBox.getZmin() - offsetZmin, geometryBoundingBox.getZmax() + offsetZmax);

	return meshBoundingBox;
}

void MeshLineCalculator::determineMeshLinesOneDirection(double min, double max, double step, std::vector<double>& coords)
{
	double numberSteps = (max - min) / step;
	size_t meshSteps = (size_t)numberSteps;
	if (numberSteps > meshSteps + 1e-3) meshSteps++;

	coords.clear();
	coords.resize(meshSteps + 1);

	for (size_t step = 0; step <= meshSteps; step++)
	{
		coords[step] = min + step * (max - min) / meshSteps;
	}
}

std::list<MeshLineCalculatorWeightedPoint> MeshLineCalculator::determineFixPlanes(int direction, double vx, double vy, double vz)
{
	// First, we iterate over all triangles and determine the ones for which the normal is aligned with the given vector (parallel and antiparallel)
	// For each of these triangles, the area is calculated and stored in the fix plane list together with the averaged triangle coordinate in the vector
	// direction.

	std::list<MeshLineCalculatorWeightedPoint> weightedFixpointList;
	
	for (auto meshEntity : meshEntities)
	{
		EntityGeometry* geometryEntity = dynamic_cast<EntityGeometry*>(meshEntity);
		if (geometryEntity != nullptr)
		{
			EntityFacetData* facetData = geometryEntity->getFacets();
			if (facetData != nullptr)
			{
				// Determine the shape priority
				double meshPriority = 0.0;

				EntityPropertiesDouble* meshPriorityProperty = dynamic_cast<EntityPropertiesDouble*>(geometryEntity->getProperties().getProperty("Mesh priority"));
				if (meshPriorityProperty != nullptr) meshPriority = meshPriorityProperty->getValue();

				CartesianMeshMaterial* material = static_cast<CartesianMeshMaterial*>(geometryEntity->getData());
				if (material != nullptr)
				{
					meshPriority += material->getPriority();
				}

				for (auto triangle : facetData->getTriangleList())
				{
					Geometry::Node n1 = facetData->getNodeVector()[triangle.getNode(0)];
					Geometry::Node n2 = facetData->getNodeVector()[triangle.getNode(1)];
					Geometry::Node n3 = facetData->getNodeVector()[triangle.getNode(2)];

					double nx, ny, nz;
					calculateTriangleNormal(n1, n2, n3, nx, ny, nz);

					if (isParallelOrAntiparallel(nx, ny, nz, nx, vy, vz, angleToleranceDeg))
					{
						// The triangle normal is parallel to the current mesh direction -> store the triangle
						double triangleArea = calculateTriangleArea(n1, n2, n3);
						double location = (n1.getCoord(direction) + n2.getCoord(direction) + n3.getCoord(direction)) / 3.0;

						MeshLineCalculatorWeightedPoint point;
						point.area     = triangleArea;
						point.coord    = location;
						point.priority = meshPriority;

						weightedFixpointList.push_back(point);
					}
				}
			}
		}
	}

	// Finally, we sort the list according to the position (the sort function sorts for the first element in the pair (and the second element if the values in the first are identical)
	weightedFixpointList.sort(
		[](const MeshLineCalculatorWeightedPoint& a,
			const MeshLineCalculatorWeightedPoint& b)
		{
			return a.coord < b.coord;
		}
	);

	return weightedFixpointList;
}

void MeshLineCalculator::calculateTriangleNormal(Geometry::Node& n1, Geometry::Node& n2, Geometry::Node& n3, double& nx, double& ny, double& nz)
{
	// Triangle edge vectors
	double ux = n2.getCoord(0) - n1.getCoord(0);
	double uy = n2.getCoord(1) - n1.getCoord(1);
	double uz = n2.getCoord(2) - n1.getCoord(2);

	double vx = n3.getCoord(0) - n1.getCoord(0);
	double vy = n3.getCoord(1) - n1.getCoord(1);
	double vz = n3.getCoord(2) - n1.getCoord(2);

	// Cross-product u x v
	nx = uy * vz - uz * vy;
	ny = uz * vx - ux * vz;
	nz = ux * vy - uy * vx;

	// Normalize normal vector
	double length = std::sqrt(nx * nx + ny * ny + nz * nz);

	if (length > 0.0)
	{
		nx /= length;
		ny /= length;
		nz /= length;
	}
}

bool MeshLineCalculator::isParallelOrAntiparallel(double nx, double ny, double nz, double vx, double vy, double vz,	double deltaAngleDeg)
{
	double deltaAngleRad  = deltaAngleDeg * M_PI / 180.0;

	double nLen = std::sqrt(nx * nx + ny * ny + nz * nz);
	double vLen = std::sqrt(vx * vx + vy * vy + vz * vz);

	if (nLen == 0.0 || vLen == 0.0)
		return false;

	double dot = nx * vx + ny * vy + nz * vz;

	// calculate the cos between the two vectors
	double cosAngle = dot / (nLen * vLen);

	// Handle numerical inaccuracies
	if (cosAngle > 1.0) cosAngle = 1.0;
	if (cosAngle < -1.0) cosAngle = -1.0;

	// parallel or antiparallel:
	// parallel      -> cosAngle close to 1
	// antiparallel  -> cosAngle close to -1

	double limit = std::cos(deltaAngleRad);

	return std::abs(cosAngle) >= limit;
}

double MeshLineCalculator::calculateTriangleArea(Geometry::Node& n1, Geometry::Node& n2, Geometry::Node& n3)
{
	// Triangle edge vectors
	double ux = n2.getCoord(0) - n1.getCoord(0);
	double uy = n2.getCoord(1) - n1.getCoord(1);
	double uz = n2.getCoord(2) - n1.getCoord(2);

	double vx = n3.getCoord(0) - n1.getCoord(0);
	double vy = n3.getCoord(1) - n1.getCoord(1);
	double vz = n3.getCoord(2) - n1.getCoord(2);

	// Cross-product u x v
	double cx = uy * vz - uz * vy;
	double cy = uz * vx - ux * vz;
	double cz = ux * vy - uy * vx;

	// magniture of Cross-product
	double crossLength = std::sqrt(cx * cx + cy * cy + cz * cz);

	// Area of triangle
	return 0.5 * crossLength;
}

std::list<MeshLineCalculatorWeightedPoint> MeshLineCalculator::mergeValuesAndAveragePositions(const std::list<MeshLineCalculatorWeightedPoint>& input, double tolerance)
{
	if (input.empty())
		return {};

	std::list<MeshLineCalculatorWeightedPoint> result;

	auto it = input.begin();

	// Start first group
	double weightedCoordSum = it->coord * it->area;
	double weightSum = it->area;
	double groupCenter = it->coord;
	double weightPriority = it->priority;

	++it;

	for (; it != input.end(); ++it)
	{
		double coord = it->coord;
		double weight = it->area;
		double priority = it->priority;

		// Check whether current value belongs to the current group
		if (std::abs(coord - groupCenter) <= tolerance)
		{
			weightedCoordSum += coord * weight;
			weightSum += weight;
			weightPriority = std::max(weightPriority, priority);

			// Update group center using weighted average
			groupCenter = weightedCoordSum / weightSum;
		}
		else
		{
			// Store completed group
			MeshLineCalculatorWeightedPoint point;
			point.area = weightSum;
			point.coord = weightedCoordSum / weightSum;
			point.priority = weightPriority;
			result.push_back(point);

			// Start new group
			weightedCoordSum = coord * weight;
			weightSum = weight;
			groupCenter = coord;
		}
	}

	// Store last group
	MeshLineCalculatorWeightedPoint point;
	point.area = weightSum;
	point.coord = weightedCoordSum / weightSum;
	point.priority = weightPriority;
	result.push_back(point);

	return result;
}

std::list<MeshLineCalculatorWeightedPoint> MeshLineCalculator::mergeWeightedPoints(std::list<MeshLineCalculatorWeightedPoint> input, double tolerance)
{
	if (input.empty())
		return {};

	std::list<MeshLineCalculatorWeightedPoint> result;

	auto it = input.begin();

	// Start first group
	double areaSum = it->area;
	double maxPriority = it->priority;

	double weightedCoordSum = it->coord * it->area;
	double priorityAreaSum = it->area;

	double groupCenter = it->coord;

	++it;

	for (; it != input.end(); ++it)
	{
		// Check whether current point belongs to current group
		if (std::abs(it->coord - groupCenter) <= tolerance)
		{
			areaSum += it->area;

			if (it->priority > maxPriority)
			{
				// New highest priority: discard previous coordinate average
				maxPriority = it->priority;
				weightedCoordSum = it->coord * it->area;
				priorityAreaSum = it->area;
			}
			else if (it->priority == maxPriority)
			{
				// Same highest priority: include in weighted coordinate average
				weightedCoordSum += it->coord * it->area;
				priorityAreaSum += it->area;
			}

			// Update group center based on selected highest-priority points
			groupCenter = weightedCoordSum / priorityAreaSum;
		}
		else
		{
			// Store completed group
			MeshLineCalculatorWeightedPoint point;
			point.coord = weightedCoordSum / priorityAreaSum;
			point.area = areaSum;
			point.priority = maxPriority;

			result.push_back(point);

			// Start new group
			areaSum = it->area;
			maxPriority = it->priority;

			weightedCoordSum = it->coord * it->area;
			priorityAreaSum = it->area;

			groupCenter = it->coord;
		}
	}

	// Store last group
	MeshLineCalculatorWeightedPoint point;
	point.coord = weightedCoordSum / priorityAreaSum;
	point.area = areaSum;
	point.priority = maxPriority;
	result.push_back(point);

	return result;
}

std::vector<MeshLineCalculatorStepRange> MeshLineCalculator::determineDensityRanges(int direction, double boundingBoxMin, double boundingBoxMax, double baseStepWidth)
{
	std::vector<MeshLineCalculatorStepRange> ranges;

	// Add the background step width
	addRange(ranges, boundingBoxMin, boundingBoxMax, baseStepWidth);

	// Loop through all solids and determine the bounds and the local stepwidth. Add this as density range
	for (auto meshEntity : meshEntities)
	{
		EntityGeometry* geometryEntity = dynamic_cast<EntityGeometry*>(meshEntity);
		if (geometryEntity != nullptr)
		{
			double xmin(0.0), xmax(0.0), ymin(0.0), ymax(0.0), zmin(0.0), zmax(0.0);
			geometryEntity->getEntityBox(xmin, xmax, ymin, ymax, zmin, zmax);

			double stepWidth = getVolumeMeshStepWidth(geometryEntity, baseStepWidth);

			switch(direction)
			{
			case 0:
				addRange(ranges, xmin, xmax, stepWidth);
				break;
			case 1:
				addRange(ranges, ymin, ymax, stepWidth);
				break;
			case 2:
				addRange(ranges, zmin, zmax, stepWidth);
				break;
			default:
				assert(0); // Invalid direction
			}
		}
	}

	return ranges;
}

double MeshLineCalculator::getVolumeMeshStepWidth(EntityBase* entity, double baseStepWidth)
{
	// Determine the base mesh step width for this entity (if specified)
	EntityPropertiesDouble* meshStepWidth = dynamic_cast<EntityPropertiesDouble*>(entity->getProperties().getProperty("Maximum edge length"));
	if (meshStepWidth == nullptr)
	{
		// This is for backward compatibility with oder models only.
		meshStepWidth = dynamic_cast<EntityPropertiesDouble*>(entity->getProperties().getProperty("Mesh step width"));
	}

	double shapeMeshStepWidth = baseStepWidth;

	if (meshStepWidth != nullptr)
	{
		if (meshStepWidth->getValue() > 0.0)
		{
			shapeMeshStepWidth = meshStepWidth->getValue();
		}
	}

	return shapeMeshStepWidth;
}

void MeshLineCalculator::addRange(std::vector<MeshLineCalculatorStepRange>& ranges, double newMin, double newMax, double newStep)
{
	if (ranges.empty())
	{
		MeshLineCalculatorStepRange newRange;
		newRange.min = newMin;
		newRange.max = newMax;
		newRange.step = newStep;

		ranges.push_back(newRange);

		return;
	}

	constexpr double EPS = 1e-12;

	if (newMax <= newMin)
		return;

	std::vector<MeshLineCalculatorStepRange> result;

	for (const auto& r : ranges)
	{
		// No overlap
		if (r.max <= newMin || r.min >= newMax)
		{
			result.push_back(r);
			continue;
		}

		// Part before new range
		if (r.min < newMin)
		{
			result.push_back({ r.min, newMin, r.step });
		}

		// Overlapping part
		double overlapMin = std::max(r.min, newMin);
		double overlapMax = std::min(r.max, newMax);

		result.push_back({
			overlapMin,
			overlapMax,
			std::min(r.step, newStep)
			});

		// Part after new range
		if (r.max > newMax)
		{
			result.push_back({ newMax, r.max, r.step });
		}
	}

	std::sort(result.begin(), result.end(),
		[](const MeshLineCalculatorStepRange& a, const MeshLineCalculatorStepRange& b)
		{
			return a.min < b.min;
		});

	// Merge neighboring ranges with same step size
	ranges.clear();

	for (const auto& r : result)
	{
		if (!ranges.empty() &&
			std::abs(ranges.back().step - r.step) < EPS
			&& std::abs(ranges.back().max - r.min) < EPS)
		{
			ranges.back().max = r.max;
		}
		else
		{
			ranges.push_back(r);
		}
	}
}