//! @file PlotDataset.cpp
//! @author Alexander Kuester (alexk95)
//! @date October 2024
// ###########################################################################################################################################################################################################################################################################################################################

// OpenTwin header
#include "OTCore/Logger.h"
#include "OTWidgets/PlotBase.h"
#include "OTWidgets/PolarPlot.h"
#include "OTWidgets/QtFactory.h"
#include "OTWidgets/PlotDataset.h"
#include "OTWidgets/CartesianPlot.h"
#include "OTWidgets/PolarPlotData.h"
#include "OTWidgets/GlobalColorStyle.h"
#include "OTWidgets/CartesianPlotCurve.h"
#include "OTWidgets/CoordinateFormatConverter.h"

// Qwt header
#include <qwt_symbol.h>
#include <qwt_polar_curve.h>
#include "OTWidgets/PlotDataset.h"

ot::PlotDataset::PlotDataset(PlotBase* _ownerPlot, const Plot1DCurveCfg& _config, PlotDatasetData&& _data) :
	m_config(_config), m_data(std::move(_data))
{
	if (_ownerPlot != nullptr) {
		setOwnerPlot(_ownerPlot);
	}
}

ot::PlotDataset::~PlotDataset() {
	this->detach();
	
	if (m_polarCurve != nullptr) {
		delete m_polarCurve;
		m_polarCurve = nullptr;
		// point and data is deleted by the polar curve
	}
	if (m_cartesianCurve != nullptr) {
		delete m_cartesianCurve;
		m_cartesianCurve = nullptr;
		delete m_cartesianCurvePointSymbol;
		m_cartesianCurvePointSymbol = nullptr;
	}	
}

void ot::PlotDataset::attach(void) {
	OTAssertNullptr(m_ownerPlot);

	if (m_isAttatched) {
		//OT_LOG_WA("Dataset already attached");
		return;
	}
	m_isAttatched = true;

	AbstractPlot* plot = m_ownerPlot->getPlot();
	if (m_ownerPlot->getCurrentPlotType() == Plot1DCfg::PlotType::Cartesian)
	{
		m_cartesianCurve->attach(dynamic_cast<CartesianPlot*>(plot));
	}
	else
	{
		m_polarCurve->attach(dynamic_cast<PolarPlot*>(plot));
	}
}

void ot::PlotDataset::detach(void) {
	if (!m_isAttatched) {
		return;
	}
	m_isAttatched = false;
	if (m_cartesianCurve != nullptr)
	{
		m_cartesianCurve->detach();
	}
	else
	{
		assert(m_polarCurve != nullptr);
		m_polarCurve->detach();
	}
}

void ot::PlotDataset::setOwnerPlot(PlotBase* _ownerPlot)
{
	assert(_ownerPlot != nullptr);
	m_ownerPlot = _ownerPlot;
	if (m_ownerPlot->getCurrentPlotType() == Plot1DCfg::PlotType::Cartesian)
	{
		buildCartesianCurve();
	}
	else
	{
		assert(_ownerPlot->getConfig().getAxisQuantity() == Plot1DCfg::AxisQuantity::Complex);
		buildPolarCurve();
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// General Setter/Getter

void ot::PlotDataset::setCurveIsVisibile(bool _isVisible, bool _repaint) {
	m_config.setVisible(_isVisible);
	if (_repaint) {
		this->updateCurveVisualization();
	}
}

void ot::PlotDataset::setCurveWidth(double _penSize, bool _repaint) {
	m_config.setLinePenWidth(_penSize);
	if (_repaint) {
		this->updateCurveVisualization();
	}
}

void ot::PlotDataset::setCurveColor(const Color& _color, bool _repaint) {
	m_config.setLinePenColor(_color);
	if (_repaint) {
		this->updateCurveVisualization();
	}
}

void ot::PlotDataset::setCurvePointsVisible(bool _isVisible, bool _repaint) {
	m_config.setPointsVisible(_isVisible);
	if (_repaint) {
		this->updateCurveVisualization();
	}
}

void ot::PlotDataset::setCurvePointInnerColor(const Color& _color, bool _repaint) {
	m_config.setPointsFillColor(_color);
	if (_repaint) {
		this->updateCurveVisualization();
	}
}

void ot::PlotDataset::setCurvePointOuterColor(const Color& _color, bool _repaint) {
	m_config.setPointsOutlinePenColor(_color);
	if (_repaint) {
		this->updateCurveVisualization();
	}
}

void ot::PlotDataset::setCurvePointSize(int _size, bool _repaint) {
	m_config.setPointsSize(_size);
	if (_repaint) {
		this->updateCurveVisualization();
	}
}

void ot::PlotDataset::setCurvePointOuterColorWidth(double _size, bool _repaint) {
	m_config.setPointsOutlinePenWidth(_size);
	if (_repaint) {
		this->updateCurveVisualization();
	}
}

void ot::PlotDataset::setDimmed(bool _isDimmed, bool _repaint) {
	m_config.setDimmed(_isDimmed);
	if (_repaint) {
		this->updateCurveVisualization();
	}
}

void ot::PlotDataset::setSelected(bool _isSelected) {
	if (_isSelected == m_isSelected) {
		return;
	}

	m_isSelected = _isSelected;

	this->updateCurveVisualization();
}

ot::CartesianPlotCurve* ot::PlotDataset::getCartesianCurve() {
	if (m_cartesianCurve != nullptr) {
		return m_cartesianCurve;
	}
	else {
		assert(m_polarCurve != nullptr);
		detach();
		delete m_polarCurve;
		m_polarCurve = nullptr;
		m_polarCurvePointSymbol = nullptr; //Deleted by the curve ??
		m_polarData = nullptr;

		buildCartesianCurve();
		attach();
		return m_cartesianCurve;
	}
}

QwtPolarCurve* ot::PlotDataset::getPolarCurve(void) {
	if (m_polarCurve != nullptr) {
		return m_polarCurve;
	}
	else {
		assert(m_cartesianCurve != nullptr);
		detach();
		delete m_cartesianCurve;
		m_cartesianCurve = nullptr;
		delete m_cartesianCurvePointSymbol;
		m_cartesianCurvePointSymbol = nullptr;
		buildPolarCurve();
		attach();
		return m_polarCurve;
	}
}

// ###########################################################################################################################################################################################################################################################################################################################

// Data Setter / Getter

void ot::PlotDataset::updateCurveVisualization(void) {
	QPen linePen = QtFactory::toQPen(m_config.getLinePen());

	const ColorStyle& cs = GlobalColorStyle::instance().getCurrentStyle();
	QBrush dimmedBrush = cs.getValue(ColorStyleValueEntry::PlotCurveDimmed).toBrush();
	QPen dimmedPen(dimmedBrush, linePen.width());
	QPen invisPen(QBrush(), 0., Qt::NoPen);
	QPen outlinePen = QPen(cs.getValue(ColorStyleValueEntry::PlotCurveDimmed).toBrush(), linePen.width() * 3.);

	// Setup outline
	if (m_isSelected) {
		if (m_cartesianCurve != nullptr) {
			m_cartesianCurve->setOutlinePen(outlinePen);
		}
		else {
			//m_polarCurve->setOutlinePen(outlinePen);
		}
	}
	else {
		if (m_cartesianCurve != nullptr) {
			m_cartesianCurve->setOutlinePen(QPen(Qt::NoPen));
		}
		else {
			//m_polarCurve->setOutlinePen(QPen(Qt::NoPen));
		}
	}

	// Setup curve
	if (m_config.getVisible()) {
		if (m_config.getDimmed()) {
			if (m_cartesianCurve != nullptr) {
				m_cartesianCurve->setPen(dimmedPen);
			}
			else {
				m_polarCurve->setPen(dimmedPen);
			}
		}
		else {
			if (m_cartesianCurve != nullptr) {
				m_cartesianCurve->setPen(linePen);
			}
			else {
				m_polarCurve->setPen(linePen);
			}
		}
	}
	else {
		if (m_cartesianCurve != nullptr) {
			m_cartesianCurve->setPen(invisPen);
		}
		else {
			m_polarCurve->setPen(invisPen);
		}
	}

	// Setup points
	if (m_config.getPointsVisible()) {
		if (m_config.getDimmed()) {
			if (m_cartesianCurvePointSymbol != nullptr) {
				m_cartesianCurvePointSymbol->setPen(dimmedPen);
				m_cartesianCurvePointSymbol->setBrush(dimmedBrush);
			}
			else {
				m_polarCurvePointSymbol->setPen(dimmedPen);
				m_polarCurvePointSymbol->setBrush(dimmedBrush);
			}
		}
		else {
			QPen pointOutlinePen = QtFactory::toQPen(m_config.getPointsOutlinePen());
			QBrush pointOutlineFillBrush = QtFactory::toQBrush(m_config.getPointsFillPainter());

			if (m_cartesianCurvePointSymbol != nullptr) {
				m_cartesianCurvePointSymbol->setPen(pointOutlinePen);
				m_cartesianCurvePointSymbol->setBrush(pointOutlineFillBrush);
			}
			else {
				m_polarCurvePointSymbol->setPen(pointOutlinePen);
				m_polarCurvePointSymbol->setBrush(pointOutlineFillBrush);
			}
		}
		if (m_cartesianCurvePointSymbol != nullptr) {
			m_cartesianCurvePointSymbol->setSize(m_config.getPointsSize());
			m_cartesianCurve->setSymbol(m_cartesianCurvePointSymbol);
		}
		else {
			m_polarCurvePointSymbol->setSize(m_config.getPointsSize());
		}
	}
	else {
		if (m_cartesianCurve != nullptr)
		{
			m_cartesianCurve->setSymbol(nullptr);
		}
	}

	if (m_cartesianCurve) {
		m_cartesianCurve->setTitle(QString::fromStdString(m_config.getTitle()));
		m_cartesianCurve->setVisible(m_config.getVisible());
	}
	if (m_polarCurve) {
		m_polarCurve->setTitle(QString::fromStdString(m_config.getTitle()));
		m_polarCurve->setVisible(m_config.getVisible());
	}
}

const ot::PointsContainer ot::PlotDataset::getDisplayedPoints() {
	ot::PointsContainer points = m_CoordinateFormatConverter.defineXYPoints(m_data, m_ownerPlot->getConfig().getAxisQuantity());
	return points;
}

void ot::PlotDataset::buildCartesianCurve() {
	m_cartesianCurve = new CartesianPlotCurve(QString::fromStdString(m_config.getTitle()));
	m_cartesianCurvePointSymbol = new QwtSymbol();
	ot::PointsContainer points = m_CoordinateFormatConverter.defineXYPoints(m_data, m_ownerPlot->getConfig().getAxisQuantity());
	m_cartesianCurve->setRawSamples(points.m_xData->data(), points.m_yData->data(), m_data.getNumberOfDatapoints());
}

void ot::PlotDataset::buildPolarCurve() {
	m_polarCurve = new QwtPolarCurve(QString::fromStdString(m_config.getTitle()));
	m_polarCurvePointSymbol = new QwtSymbol();
	ot::PointsContainer points = m_CoordinateFormatConverter.defineXYPoints(m_data, m_ownerPlot->getConfig().getAxisQuantity());
	m_polarData = new PolarPlotData(points.m_xData->data(), points.m_yData->data(), m_data.getNumberOfDatapoints());
	m_polarCurve->setSymbol(m_polarCurvePointSymbol);
	m_polarCurve->setData(m_polarData);
}
