#pragma once
#define NOMINMAX

#include <osg/Geometry>
#include <osg/Geode>
#include <osg/Point>
#include <osg/BlendFunc>
#include <osg/Depth>
#include <osg/Program>
#include <osg/Shader>
#include <osg/StateSet>
#include <osg/Hint>
#include <osg/Uniform>
#include <osg/Matrix>
#include <osgUtil/CullVisitor>
#include <OpenThreads/Mutex>
#include <OpenThreads/ScopedLock>
#include <algorithm>
#include <vector>
#include <cstdint>

#undef min
#undef max

class RubberbandPointNode : public osg::Geode
{
public:
    META_Node(osg, RubberbandPointNode);

    struct Style
    {
        // Visible (front) pass – filled disc with depth test
        osg::Vec4 visibleColor = osg::Vec4(0.12f, 0.72f, 1.0f, 0.95f);
        float     visibleSizePx = 12.0f;     // device pixels

        // Hidden (behind geometry) pass – ring without depth test
        osg::Vec4 hiddenColor = osg::Vec4(0.95f, 0.60f, 0.25f, 0.85f);
        float     hiddenSizePx = 12.0f;     // ring outer diameter in px
        float     ringThickness = 2.0f;      // ring thickness in px

        // Edge softness (anti-aliased falloff in px)
        float     edgeSoftness = 1.25f;

        // Hidden ring dashing (shader-based)
        bool      hiddenDashed = true;
        int       hiddenDashCount = 16;      // number of segments around the circle
        float     hiddenDashRatio = 0.55f;   // fraction of each segment that is "on"
        float     hiddenDashPhase = 0.0f;    // phase offset in degrees

        // Render order (hidden first, visible second)
        int       binHidden = 10;
        int       binVisible = 11;
    };

    RubberbandPointNode() { init(nullptr, Style{}); }
    explicit RubberbandPointNode(osg::Vec3Array* points, const Style& s = Style{}) { init(points, s); }

    // Required by META_Node (shallow copy)
    RubberbandPointNode(const RubberbandPointNode& rhs, const osg::CopyOp& copyop = osg::CopyOp::SHALLOW_COPY)
        : osg::Geode(rhs, copyop), _mutex()
    {
        _style = rhs._style;
        _geomHidden = rhs._geomHidden;
        _geomVisible = rhs._geomVisible;
        _daHidden = rhs._daHidden;
        _daVisible = rhs._daVisible;
        _points = rhs._points;

        _prog = rhs._prog;
        _vs = rhs._vs;
        _fs = rhs._fs;

        _coreProfile = rhs._coreProfile;
        _debugVisual = rhs._debugVisual;

        _uModeHidden = rhs._uModeHidden;
        _uModeVis = rhs._uModeVis;
        _uSizeHidden = rhs._uSizeHidden;
        _uSizeVis = rhs._uSizeVis;
        _uRingThick = rhs._uRingThick;
        _uSoftHidden = rhs._uSoftHidden;
        _uSoftVis = rhs._uSoftVis;
        _uColorHidden = rhs._uColorHidden;
        _uColorVis = rhs._uColorVis;
        _uSizePxHiddenFS = rhs._uSizePxHiddenFS;
        _uSizePxVisFS = rhs._uSizePxVisFS;

        _uDashEnabled = rhs._uDashEnabled;
        _uDashCount = rhs._uDashCount;
        _uDashRatio = rhs._uDashRatio;
        _uDashPhaseDeg = rhs._uDashPhaseDeg;

        _uMVPHidden = rhs._uMVPHidden;
        _uMVPVis = rhs._uMVPVis;

        _cullUpdater = rhs._cullUpdater;
        if (_cullUpdater.valid()) setCullCallback(_cullUpdater.get());
    }

    // ---------------- Public API ----------------

    // Provide points as osg::Vec3Array (1..N points)
    void setPoints(osg::Vec3Array* points)
    {
        OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);

        _points = points;

        _geomHidden->setVertexArray(_points.get());
        _geomVisible->setVertexArray(_points.get());

        if (_coreProfile)
        {
            // Core path: explicit attribute 0 binding for position
            _geomHidden->setVertexAttribArray(0, _points.get(), osg::Array::BIND_PER_VERTEX);
            _geomVisible->setVertexAttribArray(0, _points.get(), osg::Array::BIND_PER_VERTEX);
        }

        const GLsizei count = _points.valid() ? static_cast<GLsizei>(_points->size()) : 0;
        _daHidden->setFirst(0); _daHidden->setCount(count);
        _daVisible->setFirst(0); _daVisible->setCount(count);

        if (_points) _points->dirty();
        _geomHidden->dirtyDisplayList(); _geomHidden->dirtyBound();
        _geomVisible->dirtyDisplayList(); _geomVisible->dirtyBound();
    }

    // Consistent alias with your line class
    void setVertices(osg::Vec3Array* verts) { setPoints(verts); }

    // Copying overloads for convenience
    void setVertices(const osg::Vec3Array& verts)
    {
        osg::ref_ptr<osg::Vec3Array> copy = new osg::Vec3Array(verts);
        setPoints(copy.get());
    }
    void setVertices(const std::vector<osg::Vec3>& verts)
    {
        osg::ref_ptr<osg::Vec3Array> arr = new osg::Vec3Array();
        arr->reserve(verts.size());
        for (const auto& v : verts) arr->push_back(v);
        setPoints(arr.get());
    }

    // Update a single point in-place
    bool setVertex(size_t index, const osg::Vec3& v)
    {
        OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
        if (!_points || index >= _points->size()) return false;
        (*_points)[index] = v;
        _points->dirty();
        _geomHidden->setVertexArray(_points.get());
        _geomVisible->setVertexArray(_points.get());
        if (_coreProfile) {
            _geomHidden->setVertexAttribArray(0, _points.get(), osg::Array::BIND_PER_VERTEX);
            _geomVisible->setVertexAttribArray(0, _points.get(), osg::Array::BIND_PER_VERTEX);
        }
        _geomHidden->dirtyDisplayList();
        _geomVisible->dirtyDisplayList();
        return true;
    }

    // Mark dirty after batch edits
    void markPointsDirty()
    {
        OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
        if (_points)
        {
            _points->dirty();
            _geomHidden->setVertexArray(_points.get());
            _geomVisible->setVertexArray(_points.get());
            if (_coreProfile) {
                _geomHidden->setVertexAttribArray(0, _points.get(), osg::Array::BIND_PER_VERTEX);
                _geomVisible->setVertexAttribArray(0, _points.get(), osg::Array::BIND_PER_VERTEX);
            }
            _geomHidden->dirtyDisplayList(); _geomHidden->dirtyBound();
            _geomVisible->dirtyDisplayList(); _geomVisible->dirtyBound();
        }
    }

    // Adjust visual style
    void setStyle(const Style& s)
    {
        OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
        _style = s;
        applyStates_();
    }

    // Hidden ring dash config (shader-based)
    void setHiddenDash(bool enabled, int count = 16, float ratio = 0.55f, float phaseDeg = 0.0f)
    {
        OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
        _style.hiddenDashed = enabled;
        _style.hiddenDashCount = std::max(1, count);
        _style.hiddenDashRatio = std::min(std::max(ratio, 0.0f), 1.0f);
        _style.hiddenDashPhase = phaseDeg;

        if (_uDashEnabled.valid())  _uDashEnabled->set(enabled ? 1 : 0);
        if (_uDashCount.valid())    _uDashCount->set(_style.hiddenDashCount);
        if (_uDashRatio.valid())    _uDashRatio->set(_style.hiddenDashRatio);
        if (_uDashPhaseDeg.valid()) _uDashPhaseDeg->set(_style.hiddenDashPhase);

        _geomHidden->dirtyDisplayList();
    }

    // Force Core-Profile (GLSL 150) or Compatibility (GLSL 120) shader path manually
    void setUseCoreProfileShaders(bool useCore)
    {
        OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);

        if (_coreProfile == useCore) return;
        _coreProfile = useCore;

        // Create MVP uniforms + cull updater if entering core mode
        if (_coreProfile)
        {
            if (!_uMVPHidden.valid()) _uMVPHidden = new osg::Uniform("uMVP", osg::Matrixf());
            if (!_uMVPVis.valid())    _uMVPVis = new osg::Uniform("uMVP", osg::Matrixf());
            if (!_cullUpdater.valid())
            {
                _cullUpdater = new MVPUpdater();
                _cullUpdater->uMVPHidden = _uMVPHidden.get();
                _cullUpdater->uMVPVis = _uMVPVis.get();
                setCullCallback(_cullUpdater.get());
            }
            // Bind attribute 0 for position
            if (_points.valid())
            {
                _geomHidden->setVertexAttribArray(0, _points.get(), osg::Array::BIND_PER_VERTEX);
                _geomVisible->setVertexAttribArray(0, _points.get(), osg::Array::BIND_PER_VERTEX);
            }
        }
        else
        {
            // Back to compat: detach generic attribute 0 and callback
            _geomHidden->setVertexAttribArray(0, nullptr);
            _geomHidden->setVertexAttribBinding(0, osg::Geometry::BIND_OFF);
            _geomVisible->setVertexAttribArray(0, nullptr);
            _geomVisible->setVertexAttribBinding(0, osg::Geometry::BIND_OFF);

            setCullCallback(nullptr);
            _cullUpdater = nullptr;
        }

        rebuildProgram_();
        bindProgramToPass_(_geomHidden->getOrCreateStateSet(), /*hidden*/true);
        bindProgramToPass_(_geomVisible->getOrCreateStateSet(), /*hidden*/false);

        _geomHidden->dirtyDisplayList();
        _geomVisible->dirtyDisplayList();
    }

    // Debug: draw a solid magenta sprite regardless of ring/disc logic
    void enableDebugVisual(bool on)
    {
        OpenThreads::ScopedLock<OpenThreads::Mutex> lock(_mutex);
        _debugVisual = on;
        rebuildProgram_(); // rebuild FS with/without debug body
        bindProgramToPass_(_geomHidden->getOrCreateStateSet(), /*hidden*/true);
        bindProgramToPass_(_geomVisible->getOrCreateStateSet(), /*hidden*/false);
        _geomHidden->dirtyDisplayList();
        _geomVisible->dirtyDisplayList();
    }

private:
    // ---------------- Shader sources (120 + 150) ----------------
    // GLSL 120 (Compatibility Profile)
    static const char* VS_120()
    {
        return
            "#version 120\n"
            "uniform float uPointSize;\n"
            "void main(){\n"
            "  gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;\n"
            "  gl_PointSize = uPointSize;\n"
            "}\n";
    }
    static const char* FS_120(bool debug)
    {
        if (debug) return
            "#version 120\n"
            "void main(){ gl_FragColor = vec4(1.0,0.0,1.0,1.0); }\n";
        return
            "#version 120\n"
            "uniform int   uMode;        // 0 filled, 1 ring\n"
            "uniform vec4  uColor;\n"
            "uniform float uSizePx;\n"
            "uniform float uRingPx;\n"
            "uniform float uSoftPx;\n"
            "// Dash controls (hidden pass)\n"
            "uniform int   uDashEnabled;\n"
            "uniform int   uDashCount;\n"
            "uniform float uDashRatio;\n"
            "uniform float uDashPhaseDeg;\n"
            "float smoothDisc(vec2 uv, float soft){\n"
            "  vec2 p = uv*2.0-1.0; float r=length(p); float edge=1.0-r; return smoothstep(-soft, soft, edge);\n"
            "}\n"
            "void main(){\n"
            "  float radiusPx = 0.5 * uSizePx;\n"
            "  float soft = (uSoftPx / max(radiusPx, 1.0));\n"
            "  float a=0.0;\n"
            "  if (uMode==0){\n"
            "    a = smoothDisc(gl_PointCoord, soft);\n"
            "  } else {\n"
            "    float outer = smoothDisc(gl_PointCoord, soft);\n"
            "    float innerR = 1.0 - (2.0*uRingPx / max(uSizePx,1.0));\n"
            "    vec2 p = gl_PointCoord*2.0-1.0; float r = length(p);\n"
            "    float edgeInner = r - innerR;\n"
            "    float innerFilled = 1.0 - smoothstep(-soft, soft, -edgeInner);\n"
            "    float ringAlpha = outer * (1.0 - innerFilled);\n"
            "    if (uDashEnabled != 0 && ringAlpha > 0.0) {\n"
            "      float angle = degrees(atan(p.y, p.x));\n"
            "      angle = mod(angle - uDashPhaseDeg + 360.0, 360.0);\n"
            "      float seg   = 360.0 / max(float(uDashCount), 1.0);\n"
            "      float local = mod(angle, seg) / seg;\n"
            "      float on    = step(local, clamp(uDashRatio, 0.0, 1.0));\n"
            "      ringAlpha *= on;\n"
            "    }\n"
            "    a = ringAlpha;\n"
            "  }\n"
            "  if (a<=0.0) discard; gl_FragColor = vec4(uColor.rgb, uColor.a * a);\n"
            "}\n";
    }

    // GLSL 150 (Core Profile)
    static const char* VS_150()
    {
        return
            "#version 150\n"
            "uniform float uPointSize;\n"
            "uniform mat4  uMVP;\n"
            "in vec3 aPosition;\n"
            "void main(){ gl_Position = uMVP * vec4(aPosition,1.0); gl_PointSize = uPointSize; }\n";
    }
    static const char* FS_150(bool debug)
    {
        if (debug) return
            "#version 150\n"
            "out vec4 fragColor;\n"
            "void main(){ fragColor = vec4(1.0,0.0,1.0,1.0); }\n";
        return
            "#version 150\n"
            "uniform int   uMode;        // 0 filled, 1 ring\n"
            "uniform vec4  uColor;\n"
            "uniform float uSizePx;\n"
            "uniform float uRingPx;\n"
            "uniform float uSoftPx;\n"
            "uniform int   uDashEnabled;\n"
            "uniform int   uDashCount;\n"
            "uniform float uDashRatio;\n"
            "uniform float uDashPhaseDeg;\n"
            "out vec4 fragColor;\n"
            "float smoothDisc(vec2 uv, float soft){ vec2 p=uv*2.0-1.0; float r=length(p); float edge=1.0-r; return smoothstep(-soft,soft,edge);} \n"
            "void main(){\n"
            "  float radiusPx = 0.5 * uSizePx;\n"
            "  float soft = (uSoftPx / max(radiusPx, 1.0));\n"
            "  float a=0.0;\n"
            "  if (uMode==0){ a = smoothDisc(gl_PointCoord, soft); }\n"
            "  else{\n"
            "    float outer = smoothDisc(gl_PointCoord, soft);\n"
            "    float innerR = 1.0 - (2.0*uRingPx / max(uSizePx,1.0));\n"
            "    vec2 p = gl_PointCoord*2.0-1.0; float r = length(p);\n"
            "    float edgeInner = r - innerR;\n"
            "    float innerFilled = 1.0 - smoothstep(-soft, soft, -edgeInner);\n"
            "    float ringAlpha = outer * (1.0 - innerFilled);\n"
            "    if (uDashEnabled != 0 && ringAlpha > 0.0) {\n"
            "      float angle = degrees(atan(p.y, p.x));\n"
            "      angle = mod(angle - uDashPhaseDeg + 360.0, 360.0);\n"
            "      float seg   = 360.0 / max(float(uDashCount), 1.0);\n"
            "      float local = mod(angle, seg) / seg;\n"
            "      float on    = step(local, clamp(uDashRatio, 0.0, 1.0));\n"
            "      ringAlpha *= on;\n"
            "    }\n"
            "    a = ringAlpha;\n"
            "  }\n"
            "  if (a<=0.0) discard; fragColor = vec4(uColor.rgb, uColor.a * a);\n"
            "}\n";
    }

    // Cull-Callback: update uMVP every frame (core profile only)
    struct MVPUpdater : public osg::NodeCallback
    {
        osg::Uniform* uMVPHidden = nullptr;
        osg::Uniform* uMVPVis = nullptr;

        void operator()(osg::Node* node, osg::NodeVisitor* nv) override
        {
            auto* cv = dynamic_cast<osgUtil::CullVisitor*>(nv);
            if (cv && uMVPHidden && uMVPVis)
            {
                const osg::Matrixd& P = *cv->getProjectionMatrix();
                const osg::Matrixd& MV = *cv->getModelViewMatrix();
                const osg::Matrixf  mvpF(P * MV);
                uMVPHidden->set(mvpF);
                uMVPVis->set(mvpF);
            }
            traverse(node, nv);
        }
    };

    // Build program depending on profile & debug flag
    void rebuildProgram_()
    {
        _prog = new osg::Program();
        if (_coreProfile)
        {
            _vs = new osg::Shader(osg::Shader::VERTEX, VS_150());
            _fs = new osg::Shader(osg::Shader::FRAGMENT, FS_150(_debugVisual));
            _prog->addShader(_vs.get());
            _prog->addShader(_fs.get());
            _prog->addBindAttribLocation("aPosition", 0);
        }
        else
        {
            _vs = new osg::Shader(osg::Shader::VERTEX, VS_120());
            _fs = new osg::Shader(osg::Shader::FRAGMENT, FS_120(_debugVisual));
            _prog->addShader(_vs.get());
            _prog->addShader(_fs.get());
        }
    }

    void init(osg::Vec3Array* points, const Style& s)
    {
        _style = s;
        _coreProfile = false;    // default to compatibility shaders
        _debugVisual = false;

        // Two geometries: hidden ring (no depth), then visible filled disc (with depth)
        _geomHidden = new osg::Geometry();
        _geomVisible = new osg::Geometry();

        for (auto* g : { _geomHidden.get(), _geomVisible.get() }) {
            g->setDataVariance(osg::Object::DYNAMIC);
            g->setUseDisplayList(false);
            g->setUseVertexBufferObjects(true);
            addDrawable(g);
        }

        _daHidden = new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0,
            points ? static_cast<GLsizei>(points->size()) : 0);
        _daVisible = new osg::DrawArrays(osg::PrimitiveSet::POINTS, 0,
            points ? static_cast<GLsizei>(points->size()) : 0);
        _geomHidden->addPrimitiveSet(_daHidden.get());
        _geomVisible->addPrimitiveSet(_daVisible.get());

        if (points) _points = points;
        _geomHidden->setVertexArray(_points.get());
        _geomVisible->setVertexArray(_points.get());

        // Build initial program (compatibility path)
        rebuildProgram_();

        // Uniforms shared/setup per pass
        _uModeHidden = new osg::Uniform("uMode", 1); // 1 = ring
        _uModeVis = new osg::Uniform("uMode", 0); // 0 = filled

        _uSizeHidden = new osg::Uniform("uPointSize", _style.hiddenSizePx);
        _uSizeVis = new osg::Uniform("uPointSize", _style.visibleSizePx);

        _uRingThick = new osg::Uniform("uRingPx", _style.ringThickness);
        _uSoftHidden = new osg::Uniform("uSoftPx", _style.edgeSoftness);
        _uSoftVis = new osg::Uniform("uSoftPx", _style.edgeSoftness);

        _uColorHidden = new osg::Uniform("uColor", _style.hiddenColor);
        _uColorVis = new osg::Uniform("uColor", _style.visibleColor);

        _uSizePxHiddenFS = new osg::Uniform("uSizePx", _style.hiddenSizePx);
        _uSizePxVisFS = new osg::Uniform("uSizePx", _style.visibleSizePx);

        // Dash uniforms (hidden pass only)
        _uDashEnabled = new osg::Uniform("uDashEnabled", _style.hiddenDashed ? 1 : 0);
        _uDashCount = new osg::Uniform("uDashCount", _style.hiddenDashCount);
        _uDashRatio = new osg::Uniform("uDashRatio", _style.hiddenDashRatio);
        _uDashPhaseDeg = new osg::Uniform("uDashPhaseDeg", _style.hiddenDashPhase);

        // osg::Point (helps on compatibility profile)
        osg::ref_ptr<osg::Point> ptHidden = new osg::Point(_style.hiddenSizePx);
        osg::ref_ptr<osg::Point> ptVisible = new osg::Point(_style.visibleSizePx);

        // -------- Hidden pass (ring, no depth) --------
        {
            osg::StateSet* ss = _geomHidden->getOrCreateStateSet();
            bindProgramToPass_(ss, /*hidden*/true);

            ss->setAttributeAndModes(ptHidden.get(), osg::StateAttribute::ON);
            ss->setMode(GL_BLEND, osg::StateAttribute::ON);
            ss->setAttributeAndModes(new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA), osg::StateAttribute::ON);
            ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

            ss->setMode(GL_PROGRAM_POINT_SIZE, osg::StateAttribute::ON);
            ss->setMode(GL_POINT_SPRITE, osg::StateAttribute::ON); // harmless/ignored in some drivers

            ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::OFF); // always visible
            ss->setRenderBinDetails(_style.binHidden, "RenderBin");
        }

        // -------- Visible pass (filled disc, with depth) --------
        {
            osg::StateSet* ss = _geomVisible->getOrCreateStateSet();
            bindProgramToPass_(ss, /*hidden*/false);

            ss->setAttributeAndModes(ptVisible.get(), osg::StateAttribute::ON);
            ss->setMode(GL_BLEND, osg::StateAttribute::ON);
            ss->setAttributeAndModes(new osg::BlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA), osg::StateAttribute::ON);
            ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

            osg::ref_ptr<osg::Depth> depth = new osg::Depth();
            depth->setWriteMask(true);
            depth->setFunction(osg::Depth::LEQUAL);
            ss->setMode(GL_DEPTH_TEST, osg::StateAttribute::ON);
            ss->setAttributeAndModes(depth.get(), osg::StateAttribute::ON);

            ss->setMode(GL_PROGRAM_POINT_SIZE, osg::StateAttribute::ON);
            ss->setMode(GL_POINT_SPRITE, osg::StateAttribute::ON);

            ss->setRenderBinDetails(_style.binVisible, "RenderBin");
        }

        if (_points) _points->dirty();
        _geomHidden->dirtyBound();
        _geomVisible->dirtyBound();
    }

    void bindProgramToPass_(osg::StateSet* ss, bool hiddenPass)
    {
        ss->setAttributeAndModes(_prog.get(), osg::StateAttribute::ON);

        if (hiddenPass)
        {
            ss->addUniform(_uModeHidden.get());
            ss->addUniform(_uSizeHidden.get());      // VS: gl_PointSize
            ss->addUniform(_uSizePxHiddenFS.get());  // FS: uSizePx
            ss->addUniform(_uRingThick.get());
            ss->addUniform(_uSoftHidden.get());
            ss->addUniform(_uColorHidden.get());
            // Dashing uniforms
            ss->addUniform(_uDashEnabled.get());
            ss->addUniform(_uDashCount.get());
            ss->addUniform(_uDashRatio.get());
            ss->addUniform(_uDashPhaseDeg.get());
            if (_coreProfile && _uMVPHidden.valid()) ss->addUniform(_uMVPHidden.get());
        }
        else
        {
            ss->addUniform(_uModeVis.get());
            ss->addUniform(_uSizeVis.get());         // VS: gl_PointSize
            ss->addUniform(_uSizePxVisFS.get());     // FS: uSizePx
            ss->addUniform(_uSoftVis.get());
            ss->addUniform(_uColorVis.get());
            if (_coreProfile && _uMVPVis.valid()) ss->addUniform(_uMVPVis.get());
        }
    }

    void applyStates_()
    {
        // Hidden
        {
            osg::StateSet* ss = _geomHidden->getOrCreateStateSet();
            _uSizeHidden->set(_style.hiddenSizePx);
            _uSizePxHiddenFS->set(_style.hiddenSizePx);
            _uRingThick->set(_style.ringThickness);
            _uSoftHidden->set(_style.edgeSoftness);
            _uColorHidden->set(_style.hiddenColor);

            // Dash uniforms
            _uDashEnabled->set(_style.hiddenDashed ? 1 : 0);
            _uDashCount->set(std::max(1, _style.hiddenDashCount));
            _uDashRatio->set(std::min(std::max(_style.hiddenDashRatio, 0.0f), 1.0f));
            _uDashPhaseDeg->set(_style.hiddenDashPhase);

            if (auto* pt = dynamic_cast<osg::Point*>(ss->getAttribute(osg::StateAttribute::POINT)))
                pt->setSize(_style.hiddenSizePx);
            ss->setRenderBinDetails(_style.binHidden, "RenderBin");
        }

        // Visible
        {
            osg::StateSet* ss = _geomVisible->getOrCreateStateSet();
            _uSizeVis->set(_style.visibleSizePx);
            _uSizePxVisFS->set(_style.visibleSizePx);
            _uSoftVis->set(_style.edgeSoftness);
            _uColorVis->set(_style.visibleColor);
            if (auto* pt = dynamic_cast<osg::Point*>(ss->getAttribute(osg::StateAttribute::POINT)))
                pt->setSize(_style.visibleSizePx);
            ss->setRenderBinDetails(_style.binVisible, "RenderBin");
        }

        _geomHidden->dirtyDisplayList();
        _geomVisible->dirtyDisplayList();
    }

private:
    OpenThreads::Mutex                _mutex;

    osg::ref_ptr<osg::Geometry>       _geomHidden;
    osg::ref_ptr<osg::Geometry>       _geomVisible;

    osg::ref_ptr<osg::DrawArrays>     _daHidden;
    osg::ref_ptr<osg::DrawArrays>     _daVisible;

    osg::ref_ptr<osg::Vec3Array>      _points;

    // Shader program + sources
    osg::ref_ptr<osg::Program>        _prog;
    osg::ref_ptr<osg::Shader>         _vs;
    osg::ref_ptr<osg::Shader>         _fs;

    // Profile & debug
    bool                              _coreProfile = false; // default: compat shaders
    bool                              _debugVisual = false; // magenta test flag

    // Uniforms (per pass)
    osg::ref_ptr<osg::Uniform>        _uModeHidden, _uModeVis; // 1=ring, 0=filled
    osg::ref_ptr<osg::Uniform>        _uSizeHidden, _uSizeVis; // gl_PointSize (VS)
    osg::ref_ptr<osg::Uniform>        _uRingThick;             // ring px thickness (hidden)
    osg::ref_ptr<osg::Uniform>        _uSoftHidden, _uSoftVis; // edge softness in px
    osg::ref_ptr<osg::Uniform>        _uColorHidden, _uColorVis;
    osg::ref_ptr<osg::Uniform>        _uSizePxHiddenFS, _uSizePxVisFS; // FS: uSizePx

    // Dashing (hidden pass)
    osg::ref_ptr<osg::Uniform>        _uDashEnabled;
    osg::ref_ptr<osg::Uniform>        _uDashCount;
    osg::ref_ptr<osg::Uniform>        _uDashRatio;
    osg::ref_ptr<osg::Uniform>        _uDashPhaseDeg;

    // Core-profile MVP
    osg::ref_ptr<osg::Uniform>        _uMVPHidden, _uMVPVis;
    osg::ref_ptr<MVPUpdater>          _cullUpdater;

    Style                             _style;
};
