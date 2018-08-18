#include "cam.hpp"
#include <QSet>
#include <QDir>
#include <QFileInfo>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <libv4l2.h>
#include <libv4l2rds.h>
SScCamCapability::SScCamCapability() : QVariantMap(init(QString("/dev/video0")))
{}

SScCamCapability::SScCamCapability(int device) : QVariantMap(init(QString("/dev/video%1").arg(device)))
{}

SScCamCapability::SScCamCapability(const QString& device) : QVariantMap(init(device))
{}

QString SScCamCapability::fourcc2String(const quint32 pixelformat)
{
    const uchar a = (pixelformat      )&0xff,
                b = (pixelformat >>  8)&0xff,
                c = (pixelformat >> 16)&0xff,
                d = (pixelformat >> 24)&0xff;
    QString ret;
    ret+=a;
    ret+=b;
    ret+=c;
    ret+=d;
    return ret;
}

QVariantMap SScCamCapability::init(const QString& dev)
{
    int fd;
    if ((fd = open(dev.toUtf8().constData(), O_RDWR)) < 0) return QVariantMap();
    struct v4l2_capability cap;
    if (ioctl(fd, VIDIOC_QUERYCAP, &cap) < 0) { close(fd); return QVariantMap(); }

    QVariantMap ret;


    // Capabilities
    QByteArray ba0, ba1, ba2;
    for (int i=0; i<16; ++i) ba0.append(cap.driver[i]);
    for (int i=0; i<32; ++i) ba1.append(cap.card[i]);
    for (int i=0; i<32; ++i) ba2.append(cap.bus_info[i]);

    ret["DRIVER"]   = QString(ba0);
    ret["CARD"]     = QString(ba1);
    ret["BUSINFO"]  = QString(ba2);
    ret["VERSION"]  = cap.version;
    ret["CAPS"]     = cap.capabilities;
    ret["DEVCAPS"]  = cap.device_caps;

QList<quint32> pxfmts;
    // Format
    bool go_on = true;
    struct v4l2_fmtdesc fmt;
    fmt.index = 0;
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    do
    {
        if (ioctl(fd, VIDIOC_ENUM_FMT, &fmt) < 0) go_on = false;
        else
        {
            const QString prefix = QString("FORMAT%1").arg(fmt.index);
            QByteArray ba; for (int i=0; i<32; ++i) ba.append(fmt.description[i]);
            ret[prefix+"_DESCRIPTOR"]  = QString(ba);
            ret[prefix+"_FLAGS"]        = fmt.flags;
            const bool compressed = fmt.flags&V4L2_FMT_FLAG_COMPRESSED,
                       emulated   = fmt.flags&V4L2_FMT_FLAG_EMULATED;
            ret[prefix+"_COMPRESSED"] = compressed;
            ret[prefix+"_EMULATED"]   = emulated;
            pxfmts << fmt.pixelformat;
            ret[prefix+"_PIXELFORMAT"]   = fourcc2String(fmt.pixelformat);

            ++fmt.index;
            if (fmt.index==0) go_on = false;    //< only theoretically possible
        }

    } while (go_on);
    ret["FORMATS"] = fmt.index;

    QStringList sl;
    foreach(quint32 pxfmt, pxfmts) sl << fourcc2String(pxfmt);
    ret["FORMATIDS"] = sl;


    foreach(quint32 pxfmt, pxfmts)
    {
        struct v4l2_frmsizeenum frmsz;
        frmsz.index=0;
        frmsz.pixel_format=pxfmt;
        go_on = true;
        do
        {
            if (ioctl(fd, VIDIOC_ENUM_FRAMESIZES, &frmsz) < 0) go_on = false;
            else
            {
                QString token = QString("FRAMESIZE_%1").arg(fourcc2String(pxfmt)), value;
                switch(frmsz.type)
                {
                    case V4L2_FRMSIZE_TYPE_DISCRETE:
                    value = QString("W %1 H %2")
                            .arg(frmsz.discrete.width)
                            .arg(frmsz.discrete.height);
                    break;
                    case V4L2_FRMSIZE_TYPE_CONTINUOUS:
                    case V4L2_FRMSIZE_TYPE_STEPWISE:
                    value = QString("MINW %1 MAXW %2 MINH %3 MAXH %4 DLTW %5 DLTH %6")
                            .arg(frmsz.stepwise.min_width)
                            .arg(frmsz.stepwise.max_width)
                            .arg(frmsz.stepwise.min_height)
                            .arg(frmsz.stepwise.max_height)
                            .arg(frmsz.stepwise.step_width)
                            .arg(frmsz.stepwise.step_height);
                    break;
                }

                if (!value.isEmpty())
                {
                    qWarning("SUCCESS FRAMESIZE %s = %s", qPrintable(token), qPrintable(value));
                    if (!ret[token].toString().isEmpty()) ret[token] = ret[token].toString()+", "+value;
                    else ret[token]=value;
                }
                ++frmsz.index;
            }
        }
        while (go_on);
    }

    close(fd);
    return ret;
}
QString SScCamCapability::cap2String(const quint32 cap) const
{
    switch(cap)
    {
        case V4L2_CAP_VIDEO_CAPTURE:            return "Is a video capture device";
        case V4L2_CAP_VIDEO_OUTPUT:             return "Is a video output device";
        case V4L2_CAP_VIDEO_OVERLAY:            return "Can do video overlay";
        case V4L2_CAP_VBI_CAPTURE:              return "Is a raw VBI capture device";
        case V4L2_CAP_VBI_OUTPUT:               return "Is a raw VBI output device";
        case V4L2_CAP_SLICED_VBI_CAPTURE:       return "Is a sliced VBI capture device";
        case V4L2_CAP_SLICED_VBI_OUTPUT:        return "Is a sliced VBI output device";
        case V4L2_CAP_RDS_CAPTURE:              return "RDS data capture";
        case V4L2_CAP_VIDEO_OUTPUT_OVERLAY:     return "Can do video output overlay";
        case V4L2_CAP_HW_FREQ_SEEK:             return "Can do hardware frequency seek ";
        case V4L2_CAP_RDS_OUTPUT:               return "Is an RDS encoder";
        case V4L2_CAP_VIDEO_CAPTURE_MPLANE:     return "Is a video capture device that supports multiplanar formats";
        case V4L2_CAP_VIDEO_OUTPUT_MPLANE:      return "Is a video output device that supports multiplanar formats";
        case V4L2_CAP_VIDEO_M2M_MPLANE:         return "Is a video mem-to-mem device that supports multiplanar formats";
        case V4L2_CAP_VIDEO_M2M:                return "Is a video mem-to-mem device";
        case V4L2_CAP_TUNER:                    return "has a tuner";
        case V4L2_CAP_AUDIO:                    return "has audio support";
        case V4L2_CAP_RADIO:                    return "is a radio device";
        case V4L2_CAP_MODULATOR:                return "has a modulator";
        case V4L2_CAP_SDR_CAPTURE:              return "Is a SDR capture device";
        case V4L2_CAP_EXT_PIX_FORMAT:           return "Supports the extended pixel format";
        case V4L2_CAP_SDR_OUTPUT:               return "Is a SDR output device";
        case V4L2_CAP_META_CAPTURE:             return "Is a metadata capture device";
        case V4L2_CAP_READWRITE:                return "read/write systemcalls";
        case V4L2_CAP_ASYNCIO:                  return "async I/O";
        case V4L2_CAP_STREAMING:                return "streaming I/O ioctls";
        case V4L2_CAP_TOUCH:                    return "Is a touch device";
        case V4L2_CAP_DEVICE_CAPS:              return "sets device capabilities field";
        default: return QString("Unknown capability %1").arg(cap);
    }

}
QString SScCamCapability::driver    () const { return (*this)[ "DRIVER"].toString(); }
QString SScCamCapability::card      () const { return (*this)[   "CARD"].toString(); }
QString SScCamCapability::busInfo   () const { return (*this)["BUSINFO"].toString(); }
quint32 SScCamCapability::version   () const { return (*this)["VERSION"].toUInt  (); }
quint32 SScCamCapability::caps      () const { return (*this)[   "CAPS"].toUInt  (); }
quint32 SScCamCapability::devcaps   () const { return (*this)["DEVCAPS"].toUInt  (); }
quint32 SScCamCapability::formats   () const { return (*this)["FORMATS"].toUInt  (); }

quint32 SScCamCapability::formatFlags       (int fidx) const { return (*this)[QString("FORMAT%1_FLAGS").        arg(fidx)].  toUInt(); }
bool    SScCamCapability::formatCompressed  (int fidx) const { return (*this)[QString("FORMAT%1_COMPRESSED").   arg(fidx)].  toBool(); }
bool    SScCamCapability::formatEmulated    (int fidx) const { return (*this)[QString("FORMAT%1_EMULATED").     arg(fidx)].  toBool(); }
QString SScCamCapability::formatDescriptor  (int fidx) const { return (*this)[QString("FORMAT%1_DESCRIPTOR").   arg(fidx)].  toString(); }
QString SScCamCapability::formatPixelFormat (int fidx) const { return (*this)[QString("FORMAT%1_PIXELFORMAT").  arg(fidx)].  toString(); }


void SScCamCapability::dump() const
{
    const quint32 cap = caps(), dcap = devcaps();
    qWarning("Driver:        %s",  qPrintable( driver()));
    qWarning("Card:          %s",  qPrintable(   card()));
    qWarning("BusInfo:       %s",  qPrintable(busInfo()));
    qWarning("Version:       %u", version());
    qWarning("Caps:          %u", cap);
    qWarning("Devcaps:       %u", dcap);
    for (int i=0; i<32; ++i) if ( cap&(1<<i)) qWarning("Capability:    %s", qPrintable(cap2String(1<<i)));
    for (int i=0; i<32; ++i) if (dcap&(1<<i)) qWarning("DevCapability: %s", qPrintable(cap2String(1<<i)));
    for (quint32 i=0; i<formats(); ++i)
    {
        qWarning("Format %u: %s (%s)",i,qPrintable(formatPixelFormat(i)), qPrintable(formatDescriptor(i)));
    }
}

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


