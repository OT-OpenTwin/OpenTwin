#include "stdafx.h"

#include "HandlerIntersector.h"
#include <osg/Geometry>

#include <cassert>

HandlerIntersector::HandlerIntersector()
	: osgUtil::LineSegmentIntersector(MODEL, 0.f, 0.f)
	, m_offset(0.05f)
	, selectorDrawable(nullptr)
{
	m_hitIndices.clear();
}

HandlerIntersector::HandlerIntersector(const osg::Vec3 &start, const osg::Vec3 &end)
	: osgUtil::LineSegmentIntersector(start, end)
	, m_offset(0.05f)
	, selectorDrawable(nullptr)
{
	m_hitIndices.clear();
}

HandlerIntersector::HandlerIntersector(osgUtil::Intersector::CoordinateFrame cf, double x, double y)
	: osgUtil::LineSegmentIntersector(cf, x, y)
	, m_offset(0.05f)
	, selectorDrawable(nullptr)
{
	m_hitIndices.clear();
}

HandlerIntersector::HandlerIntersector(osgUtil::Intersector::CoordinateFrame cf, const osg::Vec3d &start, const osg::Vec3d &end)
	: osgUtil::LineSegmentIntersector(cf, start, end)
	, m_offset(0.05f)
	, selectorDrawable(nullptr)
{
}

void HandlerIntersector::setOffset(float offset)
{
	m_offset = offset;
}

float HandlerIntersector::getOffset() const
{
	return m_offset;
}

void HandlerIntersector::getHitIndices(int &first, int &last) const
{
	if (m_hitIndices.empty()){
		first = -1;
		last = -1;
	}
	else {
		first = m_hitIndices.front();
		last = m_hitIndices.back();
	}
}

osgUtil::Intersector *HandlerIntersector::clone(osgUtil::IntersectionVisitor &iv)
{
	if ( _coordinateFrame==MODEL && iv.getModelMatrix()==0 )
	{
		osg::ref_ptr<HandlerIntersector> cloned = new HandlerIntersector( _start, _end );
		cloned->_parent = this;
		cloned->m_offset = m_offset;
		return cloned.release();
	}

	osg::Matrix matrix;
	switch ( _coordinateFrame )
	{
	case WINDOW:
		if (iv.getWindowMatrix()) matrix.preMult( *iv.getWindowMatrix() );
		if (iv.getProjectionMatrix()) matrix.preMult( *iv.getProjectionMatrix() );
		if (iv.getViewMatrix()) matrix.preMult( *iv.getViewMatrix() );
		if (iv.getModelMatrix()) matrix.preMult( *iv.getModelMatrix() );
		break;
	case PROJECTION:
		if (iv.getProjectionMatrix()) matrix.preMult( *iv.getProjectionMatrix() );
		if (iv.getViewMatrix()) matrix.preMult( *iv.getViewMatrix() );
		if (iv.getModelMatrix()) matrix.preMult( *iv.getModelMatrix() );
		break;
	case VIEW:
		if (iv.getViewMatrix()) matrix.preMult( *iv.getViewMatrix() );
		if (iv.getModelMatrix()) matrix.preMult( *iv.getModelMatrix() );
		break;
	case MODEL:
		if (iv.getModelMatrix()) matrix = *iv.getModelMatrix();
		break;
	}

	osg::Matrix inverse = osg::Matrix::inverse(matrix);
	osg::ref_ptr<HandlerIntersector> cloned = new HandlerIntersector( _start*inverse, _end*inverse );
	cloned->_parent = this;
	cloned->m_offset = m_offset;
	cloned->selectorDrawable = selectorDrawable;
	return cloned.release();
}

void HandlerIntersector::intersect(osgUtil::IntersectionVisitor &iv, osg::Drawable *drawable)
{
	if (drawable != selectorDrawable) return;

	osg::BoundingBox bb = drawable->getBoundingBox();
	bb.xMin() -= m_offset; bb.xMax() += m_offset;
	bb.yMin() -= m_offset; bb.yMax() += m_offset;
	bb.zMin() -= m_offset; bb.zMax() += m_offset;

	osg::Vec3d s(_start), e(_end);
	//if (!intersectAndClip(s, e, bb)) return;
	if (iv.getDoDummyTraversal()) return;

	osg::Geometry* geometry = drawable->asGeometry();
	if (geometry)
	{
		if (!this->isRightPrimitive(geometry)) return;

		osg::Vec3Array* vertices = dynamic_cast<osg::Vec3Array*>(geometry->getVertexArray());
		if (!vertices) return;

		double minDist = DBL_MAX;
		double intersectionRatio = 0.0;
		int    intersectionIndex = 0;
		double prevSegmentLength = 0.0;

		for (unsigned int i=0; i<vertices->size()-1; i++)
		{
			int j = i + 1;
			double segmentIntersectionRatio = 0.0;
			double distance = getSkewLinesDistance(s,e,(*vertices)[i], (*vertices)[j], segmentIntersectionRatio);

			if (distance < minDist)
			{
				minDist = distance;
				intersectionRatio = segmentIntersectionRatio;
				intersectionIndex = i;
			}

			osg::Vec3d segmentVector = (*vertices)[j] - (*vertices)[i];
			prevSegmentLength += segmentVector.length();
		}

		if (minDist < DBL_MAX)
		{
			Intersection hit;
			hit.ratio = intersectionRatio;
			hit.indexList.push_back(intersectionIndex);
			hit.nodePath = iv.getNodePath();
			hit.drawable = drawable;
			hit.matrix = iv.getModelMatrix();
			insertIntersection(hit);
		}
	}
}

bool HandlerIntersector::isVirtualIntersector() const
{
	return false;
}

double HandlerIntersector::getSkewLinesDistance(const osg::Vec3d &r1, const osg::Vec3d &r2, const osg::Vec3d &v1, const osg::Vec3d &v2, double &intersectionRatio)
{
	osg::Vec3d u1 = r2-r1;
	osg::Vec3d u2 = v2-v1;
	osg::Vec3d w = v1-r1;
	
	double R1 = u1 * u1;   
	double R2 = u2 * u2;  
	double d4321 = u1 * u2; 
	double d3121 = u1 * w;
	double d4331 = w * u2; 
	double den = d4321 * d4321 - R1 * R2;
	if (den == 0.0) return 1000;
	assert(den != 0.0);
	double s = (d4321*d4331 - R2 * d3121) / den;
	double t = (R1*d4331 - d4321 * d3121) / den;

	t = std::max(0.0, std::min(1.0, t));
	s = std::max(0.0, std::min(1.0, s));

	osg::Vec3 dps = r1 + u1 * s;
	osg::Vec3 dpt = v1 + u2 * t;

	osg::Vec3d dist = dps - dpt;
	double distance = dist.length();

	intersectionRatio = t;

	return distance;
}

bool HandlerIntersector::isRightPrimitive(const osg::Geometry *geometry)
{
	const osg::Geometry::PrimitiveSetList& primitives = geometry->getPrimitiveSetList();
	for (const auto& p : primitives){
		if (p->getMode() == GL_LINE_STRIP) return true;
//		if (p->getMode() == GL_LINE_LOOP || p->getMode() == GL_LINES ||
//			p->getMode() == GL_LINE_STRIP)
//			return true;
	}
	return false;
}
