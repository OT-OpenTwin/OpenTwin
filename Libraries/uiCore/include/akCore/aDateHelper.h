#pragma once

#include <qdatetime.h>
#include <akCore/globalDataTypes.h>

namespace ak {

	class UICORE_API_EXPORT aDateHelper {

	public:
		static QTime addSeconds(const QTime & _orig, int _sec, bool & _overflow);

		static QTime addTime(const QTime & _orig, int _h, int _m, int _s, bool & _overflow);

		static QTime addTime(const QTime & _orig, int _h, int _m, int _s, int _ms, bool & _overflow);

		static QTime addTime(const QTime & _orig, const QTime & _other, bool & _overflow);

		static QTime subTime(const QTime & _orig, const QTime & _other, bool & _overflow);

		static QTime timeDif(const QTime & _orig, const QTime & _other);

		static QTime timeDif(const QTime & _orig, const QTime & _other, bool & _overflow);

	};

}
