
#ifndef HANDLERINTERSECTOR_H
#define HANDLERINTERSECTOR_H

#include <vector>
#include <osgUtil/LineSegmentIntersector>

/*! \class HandlerIntersector
* \brief A class that allows to catch intersections with line loops and lines OpenGL types.
* It uses shortest distance between the cast ray and the geometry line which is calculated
* as a distance between skew lines.
* In addition, it filters out the geometries whose primitive sets are different than line-types.
*/

class HandlerIntersector : public osgUtil::LineSegmentIntersector
{
public:
	HandlerIntersector();

	HandlerIntersector(const osg::Vec3& start, const osg::Vec3& end);
	HandlerIntersector(CoordinateFrame cf, double x, double y);
	HandlerIntersector(CoordinateFrame cf, const osg::Vec3d& start, const osg::Vec3d& end);

	void setOffset(float offset);
	float getOffset() const;
	void getHitIndices(int& first, int& last) const;

	virtual Intersector* clone( osgUtil::IntersectionVisitor& iv );
	virtual void intersect(osgUtil::IntersectionVisitor& iv, osg::Drawable* drawable);

	bool isVirtualIntersector() const;

	void setHandlerDrawable(osg::Drawable *handler) { selectorDrawable = handler; }

protected:
	double getSkewLinesDistance(const osg::Vec3d &r1, const osg::Vec3d &r2, const osg::Vec3d &v1, const osg::Vec3d &v2, double &intersectionRatio);
	virtual bool isRightPrimitive(const osg::Geometry* geometry);

	float m_offset;
	std::vector<unsigned int> m_hitIndices;
	osg::Drawable *selectorDrawable;
};

#endif // L
