// @otlicense

#pragma once

class Timeouts {
public:
	static const int connection = 30000; //! \brief Default connection timeout in ms.
	static const int tickRate = 10; //! \brief 

private:
	Timeouts() = delete;
	Timeouts(const Timeouts&) = delete;
};