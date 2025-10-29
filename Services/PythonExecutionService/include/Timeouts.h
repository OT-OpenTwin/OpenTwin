// @otlicense

#pragma once

class Timeouts {
public:
	static const int connectionTimeout = 30000; //! \brief Connection timeout in ms
	static const int responseTimeout = 30000; //! \brief Response timeout in ms
	static const int defaultTickTime = 10; //! \brief Default thread sleep time.
};