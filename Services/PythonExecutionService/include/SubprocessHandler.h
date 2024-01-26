#pragma once
#include "OTServiceFoundation/BusinessLogicHandler.h"

#include <QtWidgets/qapplication.h>
#include <QtWidgets/qmainwindow.h>

#include <QtNetwork/qlocalserver.h>
#include <QtNetwork/qlocalsocket.h>
#include <QtCore/qeventloop.h>
#include <QtCore/qobject.h>
#include <QtCore/qprocess.h>


class SubprocessHandler : public BusinessLogicHandler
{
public:
	SubprocessHandler();
};
