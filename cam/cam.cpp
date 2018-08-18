#include "cam.hpp"
#include <QSet>
#include <QDir>
#include <QFileInfo>


QList<int> SSnCam::devices()
{
    QSet <int> idxset;
    foreach(const QFileInfo& fi, QDir("/dev/").entryInfoList(QDir::System))
    {
        const QString fn = fi.fileName();
        if (fn.startsWith("video"))
        {
            bool ok = false;
            const int nr = fn.right(fn.size()-5).toInt(&ok);
            if (ok && (nr>=0)) idxset << nr;
        }
    }
    QList<int> retValue = idxset.toList();
    std::sort(retValue.begin(),retValue.end());
    return retValue;
}

QStringList SSnCam::deviceNames()
{
    QStringList retValue;
    foreach(const int idx, SSnCam::devices()) retValue << QString("/dev/video%1").arg(idx);
    return retValue;
}


