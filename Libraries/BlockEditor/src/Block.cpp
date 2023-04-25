// OpenTwin header
#include <openTwin/Block.h>

ot::Block::Block() {}

ot::Block::~Block() {}

QRectF ot::Block::boundingRect(void) const {
	return QRectF(
		QPointF(pos().x() - (blockWidth() / 2), pos().y() - (blockHeigth() / 2)), 
		QSizeF(blockWidth(), blockHeigth())
	);
}