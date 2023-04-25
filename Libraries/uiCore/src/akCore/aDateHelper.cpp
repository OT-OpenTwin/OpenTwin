#include <akCore/aDateHelper.h>

QTime ak::aDateHelper::addSeconds(const QTime & _orig, int _sec, bool & _overflow) {
	int h = _orig.hour();
	int m = _orig.minute();
	int s = _orig.second();
	int ms = _orig.msec();

	_overflow = false;

	s += _sec;

	while (s < 0) {
		s += 60;
		m--;
	}
	while (s > 59) {
		s -= 60;
		m++;
	}

	while (m < 0) {
		m += 60;
		h--;
	}
	while (m > 59) {
		m -= 60;
		h++;
	}

	while (h < 0) {
		_overflow = true;
		h += 24;
	}
	
	while (h > 23) {
		_overflow = true;
		h -= 24;
	}

	return QTime(h, m, s, ms);

}

QTime ak::aDateHelper::addTime(const QTime & _orig, int _h, int _m, int _s, bool & _overflow) {
	return addTime(_orig, _h, _m, _s, 0, _overflow);
}

QTime ak::aDateHelper::addTime(const QTime & _orig, int _h, int _m, int _s, int _ms, bool & _overflow) {
	int h = _orig.hour();
	int m = _orig.minute();
	int s = _orig.second();
	int ms = _orig.msec();

	_overflow = false;

	ms += _ms;
	s += _s;
	m += _m;
	h += _h;

	while (s < 0) {
		s += 60;
		m--;
	}
	while (s > 59) {
		s -= 60;
		m++;
	}

	while (m < 0) {
		m += 60;
		h--;
	}
	while (m > 59) {
		m -= 60;
		h++;
	}

	while (h < 0) {
		_overflow = true;
		h += 24;
	}

	while (h > 23) {
		_overflow = true;
		h -= 24;
	}

	return QTime(h, m, s, ms);
}

QTime ak::aDateHelper::addTime(const QTime & _orig, const QTime & _other, bool & _overflow) {
	return addTime(_orig, _other.hour(), _other.minute(), _other.second(), _other.msec(), _overflow);
}

QTime ak::aDateHelper::subTime(const QTime & _orig, const QTime & _other, bool & _overflow) {
	return addTime(_orig, _other.hour() * (-1), _other.minute() * (-1), _other.second() * (-1), _other.msec() * (-1), _overflow);
}

QTime ak::aDateHelper::timeDif(const QTime & _orig, const QTime & _other) {
	bool ov;
	return timeDif(_orig, _other, ov);
}

QTime ak::aDateHelper::timeDif(const QTime & _orig, const QTime & _other, bool & _overflow) {
	int hf = _orig.hour();
	int mf = _orig.minute();
	int sf = _orig.second();
	int msf = _orig.msec();

	int ht = _other.hour();
	int mt = _other.minute();
	int st = _other.second();
	int mst = _other.msec();

	int h = 0;
	int m = 0;
	int s = 0;
	int ms = 0;

	_overflow = false;

	if (msf > mst) {
		ms = mst + (1000 - msf);
		sf++;
	}
	else {
		ms = mst - msf;
	}

	if (sf > st) {
		s = st + (60 - sf);
		mf++;
	}
	else {
		s = st - sf;
	}

	if (mf > mt) {
		m = mt + (60 - mf);
		hf++;
	}
	else {
		m = mt - mf;
	}

	if (hf > ht) {
		_overflow = true;
		h = ht + (24 - hf);
	}
	else {
		h = ht - hf;
	}

	return QTime(h, m, s, ms);
}
