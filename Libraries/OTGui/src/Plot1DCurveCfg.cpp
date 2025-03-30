//! @file Plot1DCurveCfg.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTGui/FillPainter2D.h"
#include "OTGui/Plot1DCurveCfg.h"
#include "OTGui/Painter2DFactory.h"

ot::Plot1DCurveCfg::Plot1DCurveCfg() : Plot1DCurveCfg(0, 0, std::string()) {}

ot::Plot1DCurveCfg::Plot1DCurveCfg(UID _id, UID _version, const std::string& _name) :
	BasicEntityInformation(_name, _id, _version), m_navigationId(0), m_visible(true), m_dimmed(false),
	m_linePen(1., Color(DefaultColor::Red)), m_pointsSize(5), m_pointsVisible(false), 
	m_pointsFillPainter(new FillPainter2D(DefaultColor::Lime)), m_pointsOulinePen(1., DefaultColor::Lime)
{}

ot::Plot1DCurveCfg::Plot1DCurveCfg(const Plot1DCurveCfg& _other) : m_pointsFillPainter(nullptr) {
	this->operator=(_other);
}

ot::Plot1DCurveCfg::Plot1DCurveCfg(Plot1DCurveCfg&& _other) noexcept :
	m_pointsFillPainter(nullptr)
{
	this->operator=(std::move(_other));
}

ot::Plot1DCurveCfg::~Plot1DCurveCfg() {}

ot::Plot1DCurveCfg& ot::Plot1DCurveCfg::operator=(Plot1DCurveCfg&& _other) noexcept {
	if (this != &_other) {
		BasicEntityInformation::operator=(std::move(_other));
		
		m_navigationId = _other.m_navigationId;

		m_title = std::move(_other.m_title);

		m_axisTitleX = std::move(_other.m_axisTitleX);
		m_axisUnitX = std::move(_other.m_axisUnitX);
		m_axisTitleY = std::move(_other.m_axisTitleY);
		m_axisUnitY = std::move(_other.m_axisUnitY);

		m_visible = _other.m_visible;
		m_dimmed = _other.m_dimmed;

		m_linePen = std::move(_other.m_linePen);

		m_pointsVisible = _other.m_pointsVisible;
		m_pointsSize = _other.m_pointsSize;
		m_pointsOulinePen = std::move(_other.m_pointsOulinePen);

		if (m_pointsFillPainter) {
			delete m_pointsFillPainter;
		}
		m_pointsFillPainter = _other.m_pointsFillPainter;
		_other.m_pointsFillPainter = nullptr;

		m_queryInformation = std::move(_other.m_queryInformation);
	}

	return *this;
}

ot::Plot1DCurveCfg& ot::Plot1DCurveCfg::operator=(const Plot1DCurveCfg& _other) {
	BasicEntityInformation::operator=(_other);

	if (this != &_other) {
		m_navigationId = _other.m_navigationId;
		
		m_title = _other.m_title;

		m_axisTitleX = _other.m_axisTitleX;
		m_axisUnitX = _other.m_axisUnitX;
		m_axisTitleY = _other.m_axisTitleY;
		m_axisUnitY = _other.m_axisUnitY;
		
		m_visible = _other.m_visible;
		m_dimmed = _other.m_dimmed;

		m_linePen = _other.m_linePen;
		
		m_queryInformation = _other.m_queryInformation;

		m_pointsVisible = _other.m_pointsVisible;
		m_pointsSize = _other.m_pointsSize;
		m_pointsOulinePen = _other.m_pointsOulinePen;
		this->setPointsFillPainter(_other.getPointsFillPainter()->createCopy());
	}

	return *this;
}

ot::Plot1DCurveCfg& ot::Plot1DCurveCfg::operator=(const BasicEntityInformation& _other) {
	BasicEntityInformation::operator=(_other);
	return *this;
}

void ot::Plot1DCurveCfg::addToJsonObject(ot::JsonValue& _object, ot::JsonAllocator& _allocator) const {
	
	BasicEntityInformation::addToJsonObject(_object, _allocator);

	OTAssertNullptr(m_pointsFillPainter);

	_object.AddMember("NavID", m_navigationId, _allocator);

	_object.AddMember("Title", JsonString(m_title, _allocator), _allocator);
	_object.AddMember("AxisTitleX", JsonString(m_axisTitleX, _allocator), _allocator);
	_object.AddMember("AxisUnitX", JsonString(m_axisUnitX, _allocator), _allocator);
	_object.AddMember("AxisTitleY", JsonString(m_axisTitleY, _allocator), _allocator);
	_object.AddMember("AxisUnitY", JsonString(m_axisUnitY, _allocator), _allocator);

	_object.AddMember("Visible", m_visible, _allocator);
	_object.AddMember("Dimmed", m_dimmed, _allocator);

	JsonObject penObj;
	m_linePen.addToJsonObject(penObj, _allocator);
	_object.AddMember("LinePen", penObj, _allocator);

	_object.AddMember("PointsVisible", m_pointsVisible, _allocator);
	_object.AddMember("PointsSize", m_pointsSize, _allocator);
	JsonObject pointOutPenObj;
	m_pointsOulinePen.addToJsonObject(pointOutPenObj, _allocator);
	_object.AddMember("PointOutlinePen", pointOutPenObj, _allocator);

	JsonObject pointFillPainterObj;
	m_pointsFillPainter->addToJsonObject(pointFillPainterObj, _allocator);
	_object.AddMember("PointFillPainter", pointFillPainterObj, _allocator);

	JsonObject queryInfosObj;
	m_queryInformation.addToJsonObject(queryInfosObj, _allocator);
	_object.AddMember("QueryInformation", queryInfosObj, _allocator);
}

void ot::Plot1DCurveCfg::setFromJsonObject(const ot::ConstJsonObject& _object) {
	BasicEntityInformation::setFromJsonObject(_object);

	m_navigationId = json::getUInt64(_object, "NavID");

	m_title = json::getString(_object, "Title");
	m_axisTitleX = json::getString(_object, "AxisTitleX");
	m_axisUnitX = json::getString(_object, "AxisUnitX");
	m_axisTitleY = json::getString(_object, "AxisTitleY");
	m_axisUnitY = json::getString(_object, "AxisUnitY");

	m_visible = json::getBool(_object, "Visible");
	m_dimmed = json::getBool(_object, "Dimmed");

	m_linePen.setFromJsonObject(json::getObject(_object, "LinePen"));

	m_pointsVisible = json::getBool(_object, "PointsVisible");
	m_pointsSize = json::getInt(_object, "PointsSize");
	m_pointsOulinePen.setFromJsonObject(json::getObject(_object, "PointOutlinePen"));

	m_queryInformation.setFromJsonObject(json::getObject(_object, "QueryInformation"));

	Painter2D* p = Painter2DFactory::create(json::getObject(_object, "PointFillPainter"));
	if (p) {
		this->setPointsFillPainter(p);
	}
}

const ot::Color& ot::Plot1DCurveCfg::getLinePenColor() const
{

	const ot::Painter2D* painter = m_linePen.painter();
	const ot::FillPainter2D* fillPainter = dynamic_cast<const ot::FillPainter2D*>(painter);
	if (fillPainter != nullptr)
	{
		return fillPainter->getColor();
	}
	else
	{
		throw std::exception("Colour not available for this object");
	}
}

void ot::Plot1DCurveCfg::setPointsFillColor(const Color& _color) {
	this->setPointsFillPainter(new FillPainter2D(_color));
}

void ot::Plot1DCurveCfg::setPointsFillPainter(Painter2D* _painter) {
	OTAssertNullptr(_painter);
	if (_painter == m_pointsFillPainter) {
		return;
	}

	if (m_pointsFillPainter) {
		delete m_pointsFillPainter;
	}

	m_pointsFillPainter = _painter;
}

void ot::Plot1DCurveCfg::setQueryInformation(QueryInformation _queryInformation)
{
	m_queryInformation = _queryInformation;
}

const ot::QueryInformation& ot::Plot1DCurveCfg::getQueryInformation() const
{
	return m_queryInformation;
}
