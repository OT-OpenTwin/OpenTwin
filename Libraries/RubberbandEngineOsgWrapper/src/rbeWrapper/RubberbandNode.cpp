#include "rbeWrapper/RubberbandNode.h"

#include <osg/BlendFunc>
#include <osg/LineWidth>
#include <osg/StateSet>
#include <osg/NodeVisitor>
#include <algorithm>
#include <cmath>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// --------- GlowPulseCallback ---------

bool RubberbandNode::GlowPulseCallback::run(osg::Object* object, osg::Object* data)
{
    osg::NodeVisitor* nv = dynamic_cast<osg::NodeVisitor*>(data);
    if (!nv || !mat) return traverse(object, data);

    const osg::FrameStamp* fs = nv->getFrameStamp();
    if (!fs) return traverse(object, data);

    const double t = fs->getSimulationTime(); // seconds
    const double phase01 = std::sin(2.0 * M_PI * freqHz * t) * 0.5 + 0.5; // 0..1
    const float a = static_cast<float>(minA + (maxA - minA) * phase01);

    osg::Vec4 c = baseColor; c.a() = a;
    mat->setDiffuse(osg::Material::FRONT_AND_BACK, c);
    // keep emission at full alpha so "glow" feel remains
    osg::Vec4 e(baseColor.r(), baseColor.g(), baseColor.b(), 1.0f);
    mat->setEmission(osg::Material::FRONT_AND_BACK, e);

    return traverse(object, data);
}

bool RubberbandNode::CorePulseCallback::run(osg::Object* object, osg::Object* data)
{
    osg::NodeVisitor* nv = dynamic_cast<osg::NodeVisitor*>(data);
    if (!nv || !mat) return traverse(object, data);
    const osg::FrameStamp* fs = nv->getFrameStamp();
    if (!fs) return traverse(object, data);

    const double t = fs->getSimulationTime();
    const double phase01 = std::sin(2.0 * M_PI * freqHz * t) * 0.5 + 0.5; // 0..1

    float y = (phase01 < 0.5f) ? 0.5f * std::pow(2.0f * phase01, 1.7) : 1.0f - 0.5f * std::pow(2.0f * (1.0f - phase01), 1.7);
    y = std::clamp(y, 0.0f, 1.0f);

    // Linear zwischen A und B interpolieren
    osg::Vec4 c = colA * (1.0 - phase01) + colB * phase01;

    mat->setDiffuse(osg::Material::FRONT_AND_BACK, c);
    // leichte Emission, damit die Linie "aktiv" wirkt
    osg::Vec4 e(c.r(), c.g(), c.b(), 1.0f);
    mat->setEmission(osg::Material::FRONT_AND_BACK, e);

    return traverse(object, data);
}

// --------- Helpers ---------

RubberbandNode::Theme RubberbandNode::pickThemeFromBackground(const osg::Vec4& clearColor)
{
    const float L = 0.2126f * clearColor.r() + 0.7152f * clearColor.g() + 0.0722f * clearColor.b();
    return (L > 0.6f) ? Theme::Light : Theme::Dark;
}

void RubberbandNode::configureCommon(osg::StateSet* ss) const
{
    ss->setMode(GL_LINE_SMOOTH, _lineSmooth ? osg::StateAttribute::ON : osg::StateAttribute::OFF);
    ss->setMode(GL_BLEND, osg::StateAttribute::ON);
    ss->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
    ss->setMode(GL_DEPTH_TEST, _overlay ? osg::StateAttribute::OFF : osg::StateAttribute::ON);
    ss->setRenderBinDetails(100, "RenderBin");
}

void RubberbandNode::setLineAppearance(osg::Geode* geode,
    float width,
    const osg::Vec4& color,
    GLenum src,
    GLenum dst) const
{
    osg::StateSet* ss = geode->getOrCreateStateSet();

    auto lw = new osg::LineWidth(width);
    ss->setAttributeAndModes(lw, osg::StateAttribute::ON);

    auto mat = new osg::Material;
    mat->setDiffuse(osg::Material::FRONT_AND_BACK, color);
    mat->setEmission(osg::Material::FRONT_AND_BACK,
        osg::Vec4(color.r(), color.g(), color.b(), 1.0f));
    ss->setAttributeAndModes(mat, osg::StateAttribute::ON);

    auto bf = new osg::BlendFunc(src, dst);
    ss->setAttributeAndModes(bf, osg::StateAttribute::ON);

    // Cache glow material for pulsing
    if (geode == _glow.get()) {
        const_cast<RubberbandNode*>(this)->_glowMat = mat;
    }
    else if (geode == _core.get()) {
        const_cast<RubberbandNode*>(this)->_coreMat = mat; // <<< neu
    }
}

static osg::ref_ptr<osg::Vec3Array> makeEmptyVec3Array()
{
    auto a = new osg::Vec3Array;
    a->setDataVariance(osg::Object::DYNAMIC);
    return a;
}

osg::Geometry* RubberbandNode::createGeometry()
{
    auto geom = new osg::Geometry;
    geom->setUseDisplayList(false);
    geom->setUseVertexBufferObjects(true);
    geom->setDataVariance(osg::Object::DYNAMIC);

    // Start with an empty vertex array; attach shared later.
    geom->setVertexArray(makeEmptyVec3Array());

    ensureDrawArraysGL_LINES(geom);
    return geom;
}

osg::Geode* RubberbandNode::createGeode()
{
    auto geode = new osg::Geode;
    geode->setDataVariance(osg::Object::DYNAMIC);
    configureCommon(geode->getOrCreateStateSet());
    return geode;
}

void RubberbandNode::ensureDrawArraysGL_LINES(osg::Geometry* geom)
{
    osg::DrawArrays* da = nullptr;
    if (geom->getNumPrimitiveSets() > 0)
        da = dynamic_cast<osg::DrawArrays*>(geom->getPrimitiveSet(0));

    if (!da) {
        geom->removePrimitiveSet(0, geom->getNumPrimitiveSets());
        da = new osg::DrawArrays(GL_LINES, 0, 0);
        geom->addPrimitiveSet(da);
    }
    else {
        da->setMode(GL_LINES);
        da->setFirst(0);
        da->setCount(0);
    }
}

GLsizei RubberbandNode::evenVertexCount(const osg::Array* a)
{
    if (!a) return 0;
    const GLsizei n = static_cast<GLsizei>(a->getNumElements());
    return (n & 1) ? (n - 1) : n; // ensure multiple of 2
}

// --------- Public API ---------

RubberbandNode::RubberbandNode(osg::Vec3Array* verts)
{
    // Geodes & geometries
    _core = createGeode();
    _glow = createGeode();
    _shadow = createGeode();

    _coreGeom = createGeometry();
    _glowGeom = createGeometry();
    _shadowGeom = createGeometry();

    _core->addDrawable(_coreGeom);
    _glow->addDrawable(_glowGeom);
    _shadow->addDrawable(_shadowGeom);

    // Draw order: shadow (back), glow (middle), core (front)
    addChild(_shadow);
    addChild(_glow);
    addChild(_core);

    // Defaults
    _lineSmooth = true;
    _overlay = true;

    setVertices(verts);
}

void RubberbandNode::setVertices(osg::Vec3Array* verts)
{
    _vertsShared = verts; // ref_ptr shared ownership (may be nullptr)

    // All layers share the SAME vertex array
    if (_vertsShared.valid()) {
        _coreGeom->setVertexArray(_vertsShared.get());
        _glowGeom->setVertexArray(_vertsShared.get());
        _shadowGeom->setVertexArray(_vertsShared.get());
    }
    else {
        _coreGeom->setVertexArray(makeEmptyVec3Array());
        _glowGeom->setVertexArray(makeEmptyVec3Array());
        _shadowGeom->setVertexArray(makeEmptyVec3Array());
    }

    ensureDrawArraysGL_LINES(_coreGeom);
    ensureDrawArraysGL_LINES(_glowGeom);
    ensureDrawArraysGL_LINES(_shadowGeom);

    markVerticesDirty();
}

void RubberbandNode::markVerticesDirty()
{
    auto sync = [](osg::Geometry* g) {
        auto* da = dynamic_cast<osg::DrawArrays*>(g->getPrimitiveSet(0));
        const GLsizei cnt = RubberbandNode::evenVertexCount(g->getVertexArray());
        if (da) { da->setFirst(0); da->setCount(cnt); }
        if (auto* va = dynamic_cast<osg::Array*>(g->getVertexArray())) va->dirty();
        g->dirtyBound();
        };

    sync(_coreGeom);
    sync(_glowGeom);
    sync(_shadowGeom);
}

void RubberbandNode::applyTheme(Theme theme, const osg::Vec4& clearColorForAuto)
{
    _currentTheme = (theme == Theme::Auto) ? pickThemeFromBackground(clearColorForAuto)
        : theme;

    // Re-apply base states (in case toggles changed)
    configureCommon(_core->getOrCreateStateSet());
    configureCommon(_glow->getOrCreateStateSet());
    configureCommon(_shadow->getOrCreateStateSet());

    if (_currentTheme == Theme::Dark)
    {
        // Dark background: bright red core + additive orange-red glow; no shadow.
        _shadow->setNodeMask(0);

        osg::Vec4 glowCol = osg::Vec4(1.0f, 0.2f, 0.0f, 0.20f); // orange-red glow
        osg::Vec4 coreCol = osg::Vec4(1.0f, 0.1f, 0.1f, 1.0f); // strong red core

        setLineAppearance(_glow.get(), 6.0f, glowCol, GL_SRC_ALPHA, GL_ONE);
        setLineAppearance(_core.get(), 2.0f, coreCol, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        _glowBaseColor = glowCol;
        _coreBaseColor = coreCol;
    }
    else
    {
        // Light background: darker red core + dark shadow + soft reddish highlight.
        _shadow->setNodeMask(~0u);

        osg::Vec4 shadowCol = osg::Vec4(0.0f, 0.0f, 0.0f, 0.25f);
        osg::Vec4 coreCol = osg::Vec4(0.8f, 0.0f, 0.0f, 1.0f);
        osg::Vec4 glowCol = osg::Vec4(1.0f, 0.4f, 0.2f, 0.25f);

        setLineAppearance(_shadow.get(), 7.0f, shadowCol, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        setLineAppearance(_core.get(), 2.2f, coreCol, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        setLineAppearance(_glow.get(), 3.6f, glowCol, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        _glowBaseColor = glowCol;
        _coreBaseColor = coreCol;
    }

    // If pulse already enabled, sync base color
    if (_glowPulseOn && _glowPulseCb.valid()) {
        _glowPulseCb->baseColor = _glowBaseColor;
    }
}

void RubberbandNode::applyLightThemeBluePreset(bool withCorePulse, float pulseHz)
{
    // Force light theme behavior
    _currentTheme = Theme::Light;

    // Reconfigure render states
    configureCommon(_core->getOrCreateStateSet());
    configureCommon(_shadow->getOrCreateStateSet());
    configureCommon(_glow->getOrCreateStateSet());

    // Disable the glow layer completely for this preset
    _glow->setNodeMask(0);

    // --- Color setup ---
    // Shadow: slightly stronger black for crisp separation on white
    //const osg::Vec4 shadowCol = osg::Vec4(0.0f, 0.0f, 0.0f, 0.35f);
    //const osg::Vec4 shadowCol = osg::Vec4(0.0f, 0.0f, 0.0f, 0.3f);
    // Core: deeper, more saturated blue for high contrast
    //const osg::Vec4 coreCol = osg::Vec4(0.05f, 0.25f, 0.85f, 1.0f);
    //const osg::Vec4 glowCol = osg::Vec4(1.00f, 0.70f, 0.30f, 0.20f);
    //const osg::Vec4 coreCol = osg::Vec4(0.85f, 0.40f, 0.00, 1.0f);

    const osg::Vec4 shadowCol = osg::Vec4(0.0f, 0.0f, 0.0f, 0.25f);
    const osg::Vec4 coreCol = osg::Vec4(0.18f, 0.40f, 0.85f, 1.0f); // cooler, darker blue

    // Ensure shadow is visible
    _glow->setNodeMask(0); // simpler 2-layer setup
    _shadow->setNodeMask(~0u);

    // Draw order: shadow > core
//    setLineAppearance(_shadow.get(), 6.0f, shadowCol, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
//    setLineAppearance(_core.get(), 2.4f, coreCol, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    setLineAppearance(_shadow.get(), 5.0f, shadowCol, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    setLineAppearance(_core.get(), 2.0f, coreCol, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Store base colors for reset and later pulse usage
    _coreBaseColor = coreCol;
    _glowBaseColor = osg::Vec4(0, 0, 0, 0); // not used here, but keep consistent

    // No glow pulse since glow is disabled
    enableGlowPulse(false);

    // Optional: subtle core pulse to keep the line "alive"
    if (withCorePulse) {
        //const osg::Vec4 colA(0.02f, 0.10f, 0.35f, 1.0f); // darker blue
        //const osg::Vec4 colB(0.10f, 0.40f, 0.90f, 1.0f); // lighter blue
        //const osg::Vec4 colA(1.0f, 0.53f, 0.00f, 1.0f); // darker orange
        //const osg::Vec4 colB(1.0f, 0.85f, 0.00f, 1.0f); // brighter orange
        const osg::Vec4 colA(0.18f, 0.40f, 0.85f, 1.0f);
        const osg::Vec4 colB(0.36f, 0.8f, 1.0f, 1.0f);
        enableCoreColorPulse(true, pulseHz, colA, colB);
    }
    else {
        enableCoreColorPulse(false);
    }

    auto disableLighting = [](osg::Geode* g)
        {
            osg::StateSet* ss = g->getOrCreateStateSet();
            ss->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
        };
    disableLighting(_core.get());
    disableLighting(_glow.get());
    disableLighting(_shadow.get());
}

void RubberbandNode::applyDarkThemeBluePreset(bool withGlowPulse,
    float glowHz,
    float glowMinA,
    float glowMaxA,
    bool withCorePulse,
    float coreHz)
{
    // Explicitly select Dark theme behavior
    _currentTheme = Theme::Dark;

    // Reconfigure base states (overlay, line smoothing, etc.)
    configureCommon(_core->getOrCreateStateSet());
    configureCommon(_glow->getOrCreateStateSet());
    configureCommon(_shadow->getOrCreateStateSet());

    // No shadow in dark theme
    _shadow->setNodeMask(0);

    // --- Color setup ---
    // Glow: bright cyan-blue additive halo
    const osg::Vec4 glowCol = osg::Vec4(0.35f, 0.85f, 1.0f, 0.20f);
    // Core: solid bright blue line
    const osg::Vec4 coreCol = osg::Vec4(0.40f, 0.90f, 1.00f, 1.0f);

    // Draw order: (shadow disabled) > glow > core
    setLineAppearance(_glow.get(), 6.0f, glowCol, GL_SRC_ALPHA, GL_ONE); // additive blending
    setLineAppearance(_core.get(), 2.0f, coreCol, GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // Store base colors for later reset (used when disabling pulses)
    _glowBaseColor = glowCol;
    _coreBaseColor = coreCol;

    // --- Optional glow pulse ---
    // Recommended on dark backgrounds for a soft breathing glow
    if (withGlowPulse) {
        enableGlowPulse(true, glowHz, glowMinA, glowMaxA);
    }
    else {
        enableGlowPulse(false);
    }

    // --- Optional core color pulse ---
    // Adds a subtle blue brightness variation; off by default
    if (withCorePulse) {
        const osg::Vec4 colA(0.30f, 0.70f, 1.00f, 1.0f); // darker blue
        const osg::Vec4 colB(0.55f, 0.95f, 1.00f, 1.0f); // lighter blue
        enableCoreColorPulse(true, coreHz, colA, colB);
    }
    else {
        enableCoreColorPulse(false);
    }
}

void RubberbandNode::setLineSmooth(bool on)
{
    _lineSmooth = on;
    configureCommon(_core->getOrCreateStateSet());
    configureCommon(_glow->getOrCreateStateSet());
    configureCommon(_shadow->getOrCreateStateSet());
}

void RubberbandNode::setOverlay(bool drawOnTop)
{
    _overlay = drawOnTop;
    configureCommon(_core->getOrCreateStateSet());
    configureCommon(_glow->getOrCreateStateSet());
    configureCommon(_shadow->getOrCreateStateSet());
}

void RubberbandNode::setVisible(bool on)
{
    setNodeMask(on ? ~0u : 0u);
}

void RubberbandNode::enableGlowPulse(bool on, float freqHz, float minAlpha, float maxAlpha)
{
    _glowPulseOn = on;

    // Ensure we have a glow material (applyTheme populates it)
    if (!_glowMat.valid()) {
        //applyTheme(_currentTheme, osg::Vec4(0, 0, 0, 1));
    }

    if (on) {
        if (!_glowPulseCb.valid()) _glowPulseCb = new GlowPulseCallback;
        _glowPulseCb->mat = _glowMat.get();
        _glowPulseCb->baseColor = _glowBaseColor;
        _glowPulseCb->freqHz = std::max(0.01, static_cast<double>(freqHz));
        _glowPulseCb->minA = std::min(minAlpha, maxAlpha);
        _glowPulseCb->maxA = std::max(minAlpha, maxAlpha);

        _glow->setUpdateCallback(_glowPulseCb);
    }
    else {
        _glow->setUpdateCallback(nullptr);
        if (_glowMat.valid()) {
            osg::Vec4 c = _glowBaseColor;
            _glowMat->setDiffuse(osg::Material::FRONT_AND_BACK, c);
            _glowMat->setEmission(osg::Material::FRONT_AND_BACK,
                osg::Vec4(c.r(), c.g(), c.b(), 1.0f));
        }
    }
}

void RubberbandNode::enableCoreColorPulse(bool on,
    float freqHz,
    osg::Vec4 colorA,
    osg::Vec4 colorB)
{
    _corePulseOn = on;

    // Ensure that the core material exists
    if (!_coreMat.valid()) {
        applyTheme(_currentTheme, osg::Vec4(0, 0, 0, 1));
    }

    if (on) {
        if (!_corePulseCb.valid()) _corePulseCb = new CorePulseCallback;
        _corePulseCb->mat = _coreMat.get();
        _corePulseCb->freqHz = std::max(0.01, static_cast<double>(freqHz));
        _corePulseCb->colA = colorA;
        _corePulseCb->colB = colorB;

        // Set the update callback to the geode node
        _core->setUpdateCallback(_corePulseCb);
    }
    else {
        // remove callback and reset color
        _core->setUpdateCallback(nullptr);
        if (_coreMat.valid()) {
            osg::Vec4 c = _coreBaseColor;
            _coreMat->setDiffuse(osg::Material::FRONT_AND_BACK, c);
            _coreMat->setEmission(osg::Material::FRONT_AND_BACK,
                osg::Vec4(c.r(), c.g(), c.b(), 1.0f));
        }
    }
}