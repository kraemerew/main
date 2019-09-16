#include <QApplication>
#include "imagedropper.hpp"

#ifndef _DEPRECATION_DISABLE
#define _DEPRECATION_DISABLE
#if (_MSC_VER >= 1400)
#pragma warning(disable: 4996)
#endif
#endif



int main(int argc, char *argv[])
{

    QApplication app(argc,argv);
    HuNetImageDropper mw;
    mw.show();
    return app.exec();
}
