#include <QApplication>
#include "hunetmainwindow.hpp"

#ifndef _DEPRECATION_DISABLE
#define _DEPRECATION_DISABLE
#if (_MSC_VER >= 1400)
#pragma warning(disable: 4996)
#endif
#endif

int main(int argc, char *argv[])
{
    QApplication app(argc,argv);
    HuNetMainWindow mw;
    mw.show();
    return app.exec();
}
