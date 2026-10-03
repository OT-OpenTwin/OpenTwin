// @otlicense

#pragma once

// OpenTwin header
#include "OTCore/CoreTypes.h"
#include "OTCore/Variable/Variable.h"

// std header
#include <vector>

namespace ot
{

	class MDF4Dataset
	{
		OT_DECL_NOCOPY(MDF4Dataset)
		OT_DECL_DEFMOVE(MDF4Dataset)
		OT_DECL_NODEFAULT(MDF4Dataset)
	public:
		MDF4Dataset(const std::string& _name);
		virtual ~MDF4Dataset() = default;

		void setName(const std::string& _name) { m_name = _name; };
		const std::string& getName() const { return m_name; };

		void setSamples(std::vector<double>&& _data) { m_samples = std::move(_data); };
		const std::vector<double>& getSamples() const { return m_samples; };
		size_t getSamplesSize() const { return m_samples.size(); };

	private:
		std::string m_name;
		std::vector<double> m_samples;

	};
}