
#include "stdafx.h"

#include "CartesianMeshTree.h"

#include "TriangleBoxOverlapTest.h"


void CartesianMeshTree::renderTriangle(float nodes[3][3], float xmin, float xmax, float ymin, float ymax, float zmin, float zmax,
									   std::vector<char> &shapeFill, size_t my, size_t mz,
									   std::vector<double> &linesX, std::vector<double> &linesY, std::vector<double> &linesZ,
									   unsigned int ixmin, unsigned int ixmax, unsigned int iymin, unsigned int iymax, unsigned int izmin, unsigned int izmax, float delta,
									   std::vector<std::map<EntityGeometry*, std::list<Geometry::Triangle*>>> *triangleInCellInformation, Geometry::Triangle &triangle, EntityGeometry *geometryEntity)
{
	// Check whether the triangle and the box intersect

	if (triangleIntersects(nodes, xmin, xmax, ymin, ymax, zmin, zmax, linesX, linesY, linesZ, ixmin, ixmax, iymin, iymax, izmin, izmax, delta))
	{
		bool splitX = (ixmax - ixmin) > 1;
		bool splitY = (iymax - iymin) > 1;
		bool splitZ = (izmax - izmin) > 1;

		if (splitX && splitY && splitZ)
		{
			int xm = (ixmax + ixmin) / 2;
			int ym = (iymax + iymin) / 2;
			int zm = (izmax + izmin) / 2;

			renderTriangle(nodes, xmin, xmax, ymin, ymax, zmin, zmax, shapeFill, my, mz, linesX, linesY, linesZ, ixmin	, xm			, iymin	, ym			, izmin	, zm			, delta, triangleInCellInformation, triangle, geometryEntity);
			renderTriangle(nodes, xmin, xmax, ymin, ymax, zmin, zmax, shapeFill, my, mz, linesX, linesY, linesZ,ixmin	, xm			, iymin	, ym			, zm			, izmax	, delta, triangleInCellInformation, triangle, geometryEntity);
			renderTriangle(nodes, xmin, xmax, ymin, ymax, zmin, zmax, shapeFill, my, mz, linesX, linesY, linesZ,ixmin	, xm			, ym			, iymax	, izmin	, zm			, delta, triangleInCellInformation, triangle, geometryEntity);
			renderTriangle(nodes, xmin, xmax, ymin, ymax, zmin, zmax, shapeFill, my, mz, linesX, linesY, linesZ,ixmin	, xm			, ym			, iymax	, zm			, izmax	, delta, triangleInCellInformation, triangle, geometryEntity);
			renderTriangle(nodes, xmin, xmax, ymin, ymax, zmin, zmax, shapeFill, my, mz, linesX, linesY, linesZ,xm				, ixmax	, iymin	, ym			, izmin	, zm			, delta, triangleInCellInformation, triangle, geometryEntity);
			renderTriangle(nodes, xmin, xmax, ymin, ymax, zmin, zmax, shapeFill, my, mz, linesX, linesY, linesZ,xm				, ixmax	, iymin	, ym			, zm			, izmax	, delta, triangleInCellInformation, triangle, geometryEntity);
			renderTriangle(nodes, xmin, xmax, ymin, ymax, zmin, zmax, shapeFill, my, mz, linesX, linesY, linesZ,xm				, ixmax	, ym			, iymax	, izmin	, zm			, delta, triangleInCellInformation, triangle, geometryEntity);
			renderTriangle(nodes, xmin, xmax, ymin, ymax, zmin, zmax, shapeFill, my, mz, linesX, linesY, linesZ,xm				, ixmax	, ym			, iymax	, zm			, izmax	, delta, triangleInCellInformation, triangle, geometryEntity);
		}
		else if (splitX && splitY)
		{
			int xm = (ixmax + ixmin) / 2;
			int ym = (iymax + iymin) / 2;

			renderTriangle(nodes, xmin, xmax, ymin, ymax, zmin, zmax, shapeFill, my, mz, linesX, linesY, linesZ,ixmin	, xm			, iymin	, ym			, izmin	, izmax, delta, triangleInCellInformation, triangle, geometryEntity);
			renderTriangle(nodes, xmin, xmax, ymin, ymax, zmin, zmax, shapeFill, my, mz, linesX, linesY, linesZ,ixmin	, xm			, ym			, iymax	, izmin	, izmax, delta, triangleInCellInformation, triangle, geometryEntity);
			renderTriangle(nodes, xmin, xmax, ymin, ymax, zmin, zmax, shapeFill, my, mz, linesX, linesY, linesZ,xm				, ixmax	, iymin	, ym			, izmin	, izmax, delta, triangleInCellInformation, triangle, geometryEntity);
			renderTriangle(nodes, xmin, xmax, ymin, ymax, zmin, zmax, shapeFill, my, mz, linesX, linesY, linesZ,xm				, ixmax	, ym			, iymax	, izmin	, izmax, delta, triangleInCellInformation, triangle, geometryEntity);
		}
		else if (splitX && splitZ)
		{
			int xm = (ixmax + ixmin) / 2;
			int zm = (izmax + izmin) / 2;

			renderTriangle(nodes, xmin, xmax, ymin, ymax, zmin, zmax, shapeFill, my, mz, linesX, linesY, linesZ, ixmin, xm	 , iymin, iymax, izmin, zm		, delta, triangleInCellInformation, triangle, geometryEntity);
			renderTriangle(nodes, xmin, xmax, ymin, ymax, zmin, zmax, shapeFill, my, mz, linesX, linesY, linesZ, ixmin, xm	 , iymin, iymax, zm		, izmax	, delta, triangleInCellInformation, triangle, geometryEntity);
			renderTriangle(nodes, xmin, xmax, ymin, ymax, zmin, zmax, shapeFill, my, mz, linesX, linesY, linesZ, xm	  , ixmax, iymin, iymax, izmin	, zm	, delta, triangleInCellInformation, triangle, geometryEntity);
			renderTriangle(nodes, xmin, xmax, ymin, ymax, zmin, zmax, shapeFill, my, mz, linesX, linesY, linesZ, xm	  , ixmax, iymin, iymax, zm		, izmax	, delta, triangleInCellInformation, triangle, geometryEntity);
		}
		else if (splitY && splitZ)
		{
			int ym = (iymax + iymin) / 2;
			int zm = (izmax + izmin) / 2;

			renderTriangle(nodes, xmin, xmax, ymin, ymax, zmin, zmax, shapeFill, my, mz, linesX, linesY, linesZ,ixmin, ixmax, iymin, ym			, izmin	, zm			, delta, triangleInCellInformation, triangle, geometryEntity);
			renderTriangle(nodes, xmin, xmax, ymin, ymax, zmin, zmax, shapeFill, my, mz, linesX, linesY, linesZ,ixmin, ixmax, iymin, ym			, zm			, izmax	, delta, triangleInCellInformation, triangle, geometryEntity);
			renderTriangle(nodes, xmin, xmax, ymin, ymax, zmin, zmax, shapeFill, my, mz, linesX, linesY, linesZ,ixmin, ixmax, ym			 , iymax	, izmin	, zm	, delta, triangleInCellInformation, triangle, geometryEntity);
			renderTriangle(nodes, xmin, xmax, ymin, ymax, zmin, zmax, shapeFill, my, mz, linesX, linesY, linesZ,ixmin, ixmax, ym			 , iymax	, zm	, izmax	, delta, triangleInCellInformation, triangle, geometryEntity);
		}
		else if (splitX)
		{
			int xm = (ixmax + ixmin) / 2;

			renderTriangle(nodes, xmin, xmax, ymin, ymax, zmin, zmax, shapeFill, my, mz, linesX, linesY, linesZ,ixmin, xm          , iymin, iymax, izmin, izmax, delta, triangleInCellInformation, triangle, geometryEntity);
			renderTriangle(nodes, xmin, xmax, ymin, ymax, zmin, zmax, shapeFill, my, mz, linesX, linesY, linesZ,xm          , ixmax, iymin, iymax, izmin, izmax, delta, triangleInCellInformation, triangle, geometryEntity);
		}
		else if (splitY)
		{
			int ym = (iymax + iymin) / 2;

			renderTriangle(nodes, xmin, xmax, ymin, ymax, zmin, zmax, shapeFill, my, mz, linesX, linesY, linesZ,ixmin, ixmax, iymin, ym		   , izmin, izmax, delta, triangleInCellInformation, triangle, geometryEntity);
			renderTriangle(nodes, xmin, xmax, ymin, ymax, zmin, zmax, shapeFill, my, mz, linesX, linesY, linesZ,ixmin, ixmax, ym			 , iymax, izmin, izmax, delta, triangleInCellInformation, triangle, geometryEntity);
		}
		else if (splitZ)
		{
			int zm = (izmax + izmin) / 2;

			renderTriangle(nodes, xmin, xmax, ymin, ymax, zmin, zmax, shapeFill, my, mz, linesX, linesY, linesZ,ixmin, ixmax, iymin, iymax, izmin, zm		   , delta, triangleInCellInformation, triangle, geometryEntity);
			renderTriangle(nodes, xmin, xmax, ymin, ymax, zmin, zmax, shapeFill, my, mz, linesX, linesY, linesZ,ixmin, ixmax, iymin, iymax, zm			 , izmax, delta, triangleInCellInformation, triangle, geometryEntity);
		}
		else
		{
			// We are the leaf of the tree -> fill the cell
			shapeFill[ixmin + iymin * my + izmin * mz] = 1;

			if (triangleInCellInformation != nullptr)
			{
				(*triangleInCellInformation)[ixmin + iymin * my + izmin * mz][geometryEntity].push_back(&triangle);
			}
		}
	}
}

bool CartesianMeshTree::triangleIntersects(float nodes[3][3], float xmin, float xmax, float ymin, float ymax, float zmin, float zmax,
										   std::vector<double> &linesX, std::vector<double> &linesY, std::vector<double> &linesZ,
									       unsigned int ixmin, unsigned int ixmax, unsigned int iymin, unsigned int iymax, unsigned int izmin, unsigned int izmax, float delta)
{
	// We first check, whether the triangle is completely outside our bounding box
	if (xmax < linesX[ixmin] - delta || xmin > linesX[ixmax] + delta) return false;
	if (ymax < linesY[iymin] - delta || ymin > linesY[iymax] + delta) return false;
	if (zmax < linesZ[izmin] - delta || zmin > linesZ[izmax] + delta) return false;

	// Now we check whether the triangle is completely inside our bounding box
	if (   xmin >= linesX[ixmin] + delta && xmax <= linesX[ixmax] - delta
		&& ymin >= linesY[iymin] + delta && ymax <= linesY[iymax] - delta
		&& zmin >= linesZ[izmin] + delta && zmax <= linesZ[izmax] - delta) return true;

	// Finally, we need to perform a real intersection check to determine whether the triangle and our box overlap

	float boxcenter[3];
	boxcenter[0] = (float) (0.5 * (linesX[ixmin] + linesX[ixmax]));
	boxcenter[1] = (float) (0.5 * (linesY[iymin] + linesY[iymax]));
	boxcenter[2] = (float) (0.5 * (linesZ[izmin] + linesZ[izmax]));

	float boxhalfsize[3];
	boxhalfsize[0] = (float) (0.5 * (linesX[ixmax] - linesX[ixmin])) + delta;
	boxhalfsize[1] = (float) (0.5 * (linesY[iymax] - linesY[iymin])) + delta;
	boxhalfsize[2] = (float) (0.5 * (linesZ[izmax] - linesZ[izmin])) + delta;

	int overlap = triBoxOverlap(boxcenter, boxhalfsize, nodes);
	return (overlap != 0);
}

