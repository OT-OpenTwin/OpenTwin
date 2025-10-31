#pragma once
#define NOMINMAX

#include <osg/Geometry>
#include <osg/Geode>
#include <osg/LineWidth>
#include <osg/LineStipple>
#include <osg/BlendFunc>
#include <osg/Depth>
#include <osg/Hint>
#include <osg/StateSet>
#include <OpenThreads/Mutex>
#include <OpenThreads/ScopedLock>
#include <algorithm>
#include <cstdint>

class RubberbandHiddenLineNode : public osg::Geode
{
public:
    META_Node(osg, RubberbandHiddenLineNode);

    struct Style {
        // Solid (visible) pass
        osg::Vec4 solidColor = osg::Vec4(0.12f, 0.72f, 1.0f, 0.95f);
        float     solidWidth = 2.0f;

        // Dashed (hidden) pass
        osg::Vec4 dashedColor = osg::Vec4(0.95f, 0.60f, 0.25f, 0.85f);
        float     dashedWidth = 2.0f;
        uint16_t  dashPattern = 0xF0F0;
        int       dashFactor = 1;

        bool      lineSmooth = true;   // GL_LINE_SMOOTH
    };

    // Default & main ctor
    RubberbandHiddenLineNode() { init(nullptr, Style{}); }
    explicit RubberbandHiddenLineNode(osg::Vec3Array* verts, const Style& style = Style{}) { init(verts, style); }

    // *** REQUIRED BY META_Node: copy ctor (SHALLOW_COPY) ***
    RubberbandHiddenLineNode(const RubberbandHiddenLineNode& rhs,
        const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY)
        : osg::Geode(rhs, copyop)
        , _mutex()
    {
        // Shallow copy of refs/state; drawables are already copied by Geode base (shallow)
        _style = rhs._style;
        _geomDashed = rhs._geomDashed;
        _geomSolid = rhs._geomSolid;
        _daDashed = rhs._daDashed;
        _daSolid = rhs._daSolid;
        _verts = rhs._verts;
        _colDashed = rhs._colDashed;
        _colSolid = rhs._colSolid;
        _lwDashed = rhs._lwDashed;
        _lwSolid = rhs._lwSolid;
        _stipple = rhs._stipple;

        // Nothing else needed; state is already bound on the drawables
    }

    virtual ~RubberbandHiddenLineNode() {}

    // ---------------- Vertex management ----------------
    void setVertices(osg::Vec3Array* verts)
    {
        OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);

        if (verts && (verts->size() % 2) != 0)
            osg::notify(osg::WARN) << "[HiddenLine] Odd vertex count; last vertex ignored.\n";

        _verts = verts;

        _geomDashed->setVertexArray(_verts.get());
        _geomSolid->setVertexArray(_verts.get());

        const GLsizei count = _verts.valid() ? static_cast<GLsizei>(_verts->size()) : 0;
        _daDashed->setFirst(0); _daDashed->setCount(count);
        _daSolid->setFirst(0); _daSolid->setCount(count);

        if (_verts) _verts->dirty();
        _geomDashed->dirtyDisplayList(); _geomDashed->dirtyBound();
        _geomSolid->dirtyDisplayList(); _geomSolid->dirtyBound();
    }

    void markVerticesDirty()
    {
        OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
        if (_verts)
        {
            _verts->dirty();
            _geomDashed->setVertexArray(_verts.get());
            _geomSolid->setVertexArray(_verts.get());
            _geomDashed->dirtyDisplayList(); _geomDashed->dirtyBound();
            _geomSolid->dirtyDisplayList(); _geomSolid->dirtyBound();
        }
    }

    // ---------------- Style ----------------
    void setStyle(const Style& s)
    {
        OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
        _style = s;
        applyStates_();
    }

private:
    void init(osg::Vec3Array* verts, const Style& style)
    {
        _style = style;

        // Create two geometries: dashed (draw FIRST), then solid
        _geomDashed = new osg::Geometry();
        _geomSolid = new osg::Geometry();

        for (auto* g : { _geomDashed.get(), _geomSolid.get() }) {
            g->setDataVariance(osg::Object::DYNAMIC);
            g->setUseDisplayList(false);
            g->setUseVertexBufferObjects(true);
            addDrawable(g);
        }

        // Primitive sets
        _daDashed = new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, verts ? static_cast<GLsizei>(verts->size()) : 0);
        _daSolid = new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, verts ? static_cast<GLsizei>(verts->size()) : 0);
        _geomDashed->addPrimitiveSet(_daDashed.get());
        _geomSolid->addPrimitiveSet(_daSolid.get());

        if (verts) _verts = verts;
        _geomDashed->setVertexArray(_verts.get());
        _geomSolid->setVertexArray(_verts.get());

        // Color arrays
        _colDashed = new osg::Vec4Array(1); (*_colDashed)[0] = _style.dashedColor;
        _colSolid = new osg::Vec4Array(1); (*_colSolid)[0] = _style.solidColor;
        _geomDashed->setColorArray(_colDashed.get(), osg::Array::BIND_OVERALL);
        _geomSolid->setColorArray(_colSolid.get(), osg::Array::BIND_OVERALL);

        // Line widths
        _lwDashed = new osg::LineWidth(_style.dashedWidth);
        _lwSolid = new osg::LineWidth(_style.solidWidth);

        applyStates_();

        if (_verts) _verts->dirty();
        _geomDashed->dirtyBound();
        _geomSolid->dirtyBound();
    }

    void applyStates_()
    {
        // ----- Dashed pass: NO depth test, dashed, drawn first -----
        {
            osg::StateSet* ss = _geomDashed->getOrCreateStateSet();
            ss->setMode(GL_BLEND, osg::StateAttribute::ON);
            ss->setAttributeAndModes(new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA), osg::StateAttribute::ON);
            ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

            ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF);        // always visible
            ss->setRenderBinDetails(10, "RenderBin");                    // draw before solid

            if (_style.lineSmooth) {
                ss->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
                ss->setAttribute(new osg::Hint(GL_LINE_SMOOTH_HINT, GL_NICEST));
            }
            else {
                ss->setMode(GL_LINE_SMOOTH, osg::StateAttribute::OFF);
            }

            // Dashes
            if (!_stipple) _stipple = new osg::LineStipple();
            _stipple->setFactor(_style.dashFactor);
            _stipple->setPattern(_style.dashPattern);
            ss->setAttributeAndModes(_stipple.get(), osg::StateAttribute::ON);

            // Width & color
            ss->setAttributeAndModes(_lwDashed.get(), osg::StateAttribute::ON);
            (*_colDashed)[0] = _style.dashedColor; _colDashed->dirty();
            _lwDashed->setWidth(_style.dashedWidth);
        }

        // ----- Solid pass: WITH depth test/write, no dash, drawn second -----
        {
            osg::StateSet* ss = _geomSolid->getOrCreateStateSet();
            ss->setMode(GL_BLEND, osg::StateAttribute::ON);
            ss->setAttributeAndModes(new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA), osg::StateAttribute::ON);
            ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

            osg::ref_ptr<osg::Depth> depth = new osg::Depth();
            depth->setWriteMask(true);
            depth->setFunction(osg::Depth::LEQUAL);
            ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
            ss->setAttributeAndModes(depth.get(), osg::StateAttribute::ON);

            ss->setRenderBinDetails(11, "RenderBin"); // draw after dashed

            if (_style.lineSmooth) {
                ss->setMode(GL_LINE_SMOOTH, osg::StateAttribute::ON);
                ss->setAttribute(new osg::Hint(GL_LINE_SMOOTH_HINT, GL_NICEST));
            }
            else {
                ss->setMode(GL_LINE_SMOOTH, osg::StateAttribute::OFF);
            }

            // Ensure no stipple for solid pass
            if (_stipple) ss->removeAttribute(_stipple.get());

            // Width & color
            ss->setAttributeAndModes(_lwSolid.get(), osg::StateAttribute::ON);
            (*_colSolid)[0] = _style.solidColor; _colSolid->dirty();
            _lwSolid->setWidth(_style.solidWidth);
        }

        _geomDashed->dirtyDisplayList();
        _geomSolid->dirtyDisplayList();
    }

private:
    OpenThreads::Mutex                _mutex;

    osg::ref_ptr<osg::Geometry>       _geomDashed;
    osg::ref_ptr<osg::Geometry>       _geomSolid;

    osg::ref_ptr<osg::DrawArrays>     _daDashed;
    osg::ref_ptr<osg::DrawArrays>     _daSolid;

    osg::ref_ptr<osg::Vec3Array>      _verts;

    osg::ref_ptr<osg::Vec4Array>      _colDashed;
    osg::ref_ptr<osg::Vec4Array>      _colSolid;

    osg::ref_ptr<osg::LineWidth>      _lwDashed;
    osg::ref_ptr<osg::LineWidth>      _lwSolid;

    osg::ref_ptr<osg::LineStipple>    _stipple;

    Style                             _style;
};
