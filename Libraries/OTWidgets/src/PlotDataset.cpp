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

// Qwt header
#include <qwt_symbol.h>
#include <qwt_plot_curve.h>
#include <qwt_polar_curve.h>

ot::PlotDataset::PlotDataset(PlotBase* _ownerPlot, const Plot1DCurveCfg& _config, PlotDatasetData&& _data) :
	m_config(_config), m_data(std::move(_data)), m_ownerPlot(_ownerPlot), m_isAttatched(false), m_polarData(nullptr)
{
	m_cartesianCurve = new QwtPlotCurve(QString::fromStdString(m_config.getTitle()));
	m_cartesianCurvePointSymbol = new QwtSymbol;
	m_cartesianCurve->setRawSamples(m_data.getDataX(), m_data.getDataY(), m_data.getDataSize());

	m_polarCurve = new QwtPolarCurve(QString::fromStdString(m_config.getTitle()));
	m_polarCurvePointSymbol = new QwtSymbol;
	m_polarData = new PolarPlotData(m_data.getDataX(), m_data.getDataY(), m_data.getDataSize());
	m_polarCurve->setSymbol(m_polarCurvePointSymbol);
	m_polarCurve->setData(m_polarData);
}

ot::PlotDataset::~PlotDataset() {
	this->detach();
	
	delete m_polarCurve;
	// point and data is deleted by the polar curve

	delete m_cartesianCurvePointSymbol;
	delete m_cartesianCurve;
}

void ot::PlotDataset::attach(void) {
	OTAssertNullptr(m_ownerPlot);

	if (m_isAttatched) {
		//OT_LOG_WA("Dataset already attached");
		return;
	}
	m_isAttatched = true;

	m_cartesianCurve->attach(m_ownerPlot->getCartesianPlot());
	m_polarCurve->attach(m_ownerPlot->getPolarPlot());
}

void ot::PlotDataset::detach(void) {
	if (!m_isAttatched) {
		return;
	}
	m_isAttatched = false;

	m_cartesianCurve->detach();
	m_polarCurve->detach();
}

void ot::PlotDataset::setOwnerPlot(PlotBase* _ownerPlot)
{
	m_ownerPlot = _ownerPlot;
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

void ot::PlotDataset::setCurveTitle(const std::string& _title) {
	m_config.setTitle(_title);
	m_cartesianCurve->setTitle(QString::fromStdString(m_config.getTitle()));
	m_polarCurve->setTitle(QString::fromStdString(m_config.getTitle()));
}

void ot::PlotDataset::setDimmed(bool _isDimmed, bool _repaint) {
	m_config.setDimmed(_isDimmed);
	if (_repaint) {
		this->updateCurveVisualization();
	}
}

void ot::PlotDataset::calculateData(Plot1DCfg::AxisQuantity _axisQuantity) {
	m_data.calculateData(_axisQuantity, m_cartesianCurve, m_polarData);
}

// ###########################################################################################################################################################################################################################################################################################################################

// Data Setter / Getter

void ot::PlotDataset::replaceData(double* _dataX, double* _dataY, long _dataSize) {
	m_data.replaceData(_dataX, nullptr, _dataY, nullptr, nullptr, _dataSize);

	m_cartesianCurve->setRawSamples(m_data.getDataX(), m_data.getDataY(), m_data.getDataSize());
	m_polarData->setData(m_data.getDataX(), m_data.getDataY(), m_data.getDataSize());
}

bool ot::PlotDataset::getDataAt(int _index, double& _x, double& _y) {
	return m_data.getDataAt(_index, _x, _y);
}

bool ot::PlotDataset::getCopyOfData(double*& _x, double*& _y, long& _size) {
	return m_data.getCopyOfData(_x, _y, _size);
}

bool ot::PlotDataset::getCopyOfYim(double*& _yim, long& _size) {
	return m_data.getCopyOfYim(_yim, _size);
}

void ot::PlotDataset::updateCurveVisualization(void) {
	QPen linePen = QtFactory::toQPen(m_config.getLinePen());

	const ColorStyle& cs = GlobalColorStyle::instance().getCurrentStyle();
	QBrush dimmedBrush = cs.getValue(ColorStyleValueEntry::PlotCurveDimmed).toBrush();
	QPen dimmedPen(dimmedBrush, linePen.width());
	QPen invisPen(QBrush(), 0., Qt::NoPen);

	// Setup curve
	if (m_config.getVisible()) {
		if (m_config.getDimmed()) {
			m_cartesianCurve->setPen(dimmedPen);
			m_polarCurve->setPen(dimmedPen);
		}
		else {
			m_cartesianCurve->setPen(linePen);
			m_polarCurve->setPen(linePen);
		}
	}
	else {
		m_cartesianCurve->setPen(invisPen);
		m_polarCurve->setPen(invisPen);
	}

	// Setup points
	if (m_config.getPointsVisible()) {

		if (m_config.getDimmed()) {
			m_cartesianCurvePointSymbol->setPen(dimmedPen);
			m_cartesianCurvePointSymbol->setBrush(dimmedBrush);

			m_polarCurvePointSymbol->setPen(dimmedPen);
			m_polarCurvePointSymbol->setBrush(dimmedBrush);
		}
		else {
			QPen pointOutlinePen = QtFactory::toQPen(m_config.getPointsOutlinePen());
			QBrush pointOutlineFillBrush = QtFactory::toQBrush(m_config.getPointsFillPainter());

			m_cartesianCurvePointSymbol->setPen(pointOutlinePen);
			m_cartesianCurvePointSymbol->setBrush(pointOutlineFillBrush);

			m_polarCurvePointSymbol->setPen(pointOutlinePen);
			m_polarCurvePointSymbol->setBrush(pointOutlineFillBrush);
		}

		m_cartesianCurvePointSymbol->setSize(m_config.getPointsSize());
		m_polarCurvePointSymbol->setSize(m_config.getPointsSize());

		m_cartesianCurve->setSymbol(m_cartesianCurvePointSymbol);
	}
	else {
		m_cartesianCurve->setSymbol(nullptr);
	}

	m_cartesianCurve->setVisible(m_config.getVisible());
	m_polarCurve->setVisible(m_config.getVisible());
}
