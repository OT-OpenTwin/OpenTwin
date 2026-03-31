// @otlicense

// OpenTwin header
#include "OTCore/Logging/Logger.h"
#include "OTWidgets/Header/TableHeader.h"
#include "OTWidgets/Widgets/Table.h"

ot::TableHeader::TableHeader(Table* _table, Qt::Orientation _orientation)
	: HeaderBase(_orientation, _table), m_table(_table)
{
	
}

ot::HeaderFilter::Features ot::TableHeader::getFilterFeatures(int _logicalIndex) const
{
	HeaderFilter::Features features;
	
	const TableHeaderItemCfg* cfg = nullptr;

	if (orientation() == Qt::Horizontal)
	{
		cfg = m_table->getHorizontalHeaderItemCfg(_logicalIndex);
		features.set(HeaderFilter::Feature::SortingEnabled, m_table->getFilterColumnSortingEnabled());
	}
	else
	{
		cfg = m_table->getVerticalHeaderItemCfg(_logicalIndex);
		features.set(HeaderFilter::Feature::SortingEnabled, m_table->getFilterRowSortingEnabled());
	}

	if (cfg)
	{
		if (cfg->getFilterBehavior() == TableHeaderItemCfg::FilterBehavior::UseText)
		{
			features.set(HeaderFilter::Feature::FilteringEnabled);
		}
		else if (cfg->getFilterBehavior() == TableHeaderItemCfg::FilterBehavior::RequestData)
		{
			features.set(HeaderFilter::Feature::FilteringEnabled);
		}
	}

	return features;
}

QString ot::TableHeader::getFilterTitle(int _logicalIndex) const
{
	const TableHeaderItemCfg* cfg = nullptr;

	if (orientation() == Qt::Horizontal)
	{
		cfg = m_table->getHorizontalHeaderItemCfg(_logicalIndex);
	}
	else
	{
		cfg = m_table->getVerticalHeaderItemCfg(_logicalIndex);
	}

	if (cfg)
	{
		return QString::fromStdString(cfg->getText());
	}
	else
	{
		return QString();
	}
}

QStringList ot::TableHeader::getFilterOptions(int _logicalIndex) const
{
	const TableHeaderItemCfg* cfg = nullptr;

	if (orientation() == Qt::Horizontal)
	{
		cfg = m_table->getHorizontalHeaderItemCfg(_logicalIndex);
	}
	else
	{
		cfg = m_table->getVerticalHeaderItemCfg(_logicalIndex);
	}


	if (cfg)
	{
		switch (cfg->getFilterBehavior())
		{
		case TableHeaderItemCfg::FilterBehavior::UseText:
			return getFilterOptionsFromText(_logicalIndex);
			
		case TableHeaderItemCfg::FilterBehavior::RequestData:
			//return m_table->getFilterOptionsForColumn(_logicalIndex);
			break;
		default:
			break;
		}
	}

	return QStringList();
}

void ot::TableHeader::sortOrderChangeRequest(int _logicalIndex, Qt::SortOrder _sortOrder)
{
	if (orientation() == Qt::Horizontal)
	{
		m_table->sortByColumn(_logicalIndex, _sortOrder);
		m_table->setContentChanged(true);
	}
	else
	{
		OT_LOG_W("Row sorting not implemented");
	}
}

void ot::TableHeader::filterOptionsChanged(int _logicalIndex, const QStringList& _selectedOptions)
{
	if (_selectedOptions.isEmpty())
	{
		if (orientation() == Qt::Horizontal)
		{
			for (int r = 0; r < m_table->rowCount(); r++)
			{
				m_table->setRowHidden(r, false);
			}
		}
		else
		{
			for (int c = 0; c < m_table->columnCount(); c++)
			{
				m_table->setColumnHidden(c, false);
			}
		}
	}
	else
	{
		if (orientation() == Qt::Horizontal)
		{
			for (int r = 0; r < m_table->rowCount(); r++)
			{
				QVariant data = m_table->model()->data(m_table->model()->index(r, _logicalIndex));
				bool match = data.isValid() && _selectedOptions.contains(data.toString());
				m_table->setRowHidden(r, !match);
			}
		}
		else
		{
			for (int c = 0; c < m_table->columnCount(); c++)
			{
				QVariant data = m_table->model()->data(m_table->model()->index(_logicalIndex, c));
				bool match = data.isValid() && _selectedOptions.contains(data.toString());
				m_table->setColumnHidden(c, !match);
			}
		}
	}
}

QStringList ot::TableHeader::getFilterOptionsFromText(int _logicalIndex) const
{
	QStringList options;

	if (orientation() == Qt::Horizontal)
	{
		options.reserve(m_table->rowCount());
		for (int r = 0; r < m_table->rowCount(); r++)
		{
			QVariant data = m_table->model()->data(m_table->model()->index(r, _logicalIndex));
			if (data.isValid())
			{
				options.push_back(data.toString());
			}
		}
	}
	else
	{
		options.reserve(m_table->columnCount());
		for (int c = 0; c < m_table->columnCount(); c++)
		{
			QVariant data = m_table->model()->data(m_table->model()->index(_logicalIndex, c));
			if (data.isValid())
			{
				options.push_back(data.toString());
			}
		}
	}

	options.removeDuplicates();
	options.shrink_to_fit();

	options.sort();

	return options;
}
